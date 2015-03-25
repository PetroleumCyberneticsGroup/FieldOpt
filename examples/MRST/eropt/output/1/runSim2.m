function [] = runSim2

mrstModule add ad-fi
mrstModule add deckformat

% input/output-files ------------------------------------------------------
caseNm  = 'SIMPLE10x5x10.txt';
cntrNm  = 'SIMPLE10x5x10_CONTROLS.TXT';
outNm   = 'SIMPLE10x5x10_RES.TXT';
gradNm  = 'SIMPLE10x5x10_GRAD.TXT';
% -------------------------------------------------------------------------

% Enable this to get convergence reports when solving schedules
mrstVerbose false

% check if initialized model already exists
[pth, nm, ext] = fileparts(caseNm);
modelNm = fullfile(pth,[nm,'.mat']);
doInitialize = isempty(dir(modelNm));
if doInitialize
    deck = readEclipseDeck(caseNm);
    % Convert to MRST units (SI)
    deck = convertDeckUnits(deck);
    % Create grid
    G = initEclipseGrid(deck);
    % Set up the rock structure
    rock  = initEclipseRock(deck);
    rock  = compressRock(rock, G.cells.indexMap);
    % Create fluid
    fluid = initDeckADIFluid(deck);
    % Get schedule
    schedule = deck.SCHEDULE;

    %% Compute constants
    G = computeGeometry(G);

    %% Set up reservoir
    % We turn on gravity and set up reservoir and scaling factors.
    gravity on
    state = initStateADI(G, fluid, deck);
    system = initADISystem({'Oil', 'Water'}, G, rock, fluid);

    % save initialized model
    save(modelNm, 'G', 'state', 'rock', 'fluid', 'schedule', 'system');
else
    load(modelNm, 'G', 'state', 'rock', 'fluid', 'schedule', 'system');
end

%if control input is given, edit schedule:
numContrSteps = numel(schedule.control);
if ~isempty(dir(cntrNm));
    fid = fopen(cntrNm);
    u   = fscanf(fid, '%g');
    numWells      = numel(u)/numContrSteps;
    vals     = mat2cell(u(:), numWells*ones(numContrSteps, 1), 1);
    schedule = updateSchedule(schedule, vals);
    fclose(fid);
end

%run simulation:
[wellSols, states] = runScheduleADI(state, G, rock, system, schedule);

%produce output
numWells = numel(wellSols{1});
numSteps = numel(wellSols);
wrats    = zeros(numWells, numSteps);
orats    = zeros(numWells, numSteps);
grats    = zeros(numWells, numSteps);
bhps     = zeros(numWells, numSteps);
for wn = 1:numWells
    wrats(wn,:) = cellfun(@(x)x(wn).qWs, wellSols)';
    orats(wn,:) = cellfun(@(x)x(wn).qOs, wellSols)';
    grats(wn,:) = cellfun(@(x)x(wn).qGs, wellSols)';
    bhps(wn,:)  = cellfun(@(x)x(wn).pressure, wellSols)';
end

% take average over controlsteps
M = sparse((1:numSteps)', schedule.step.control, schedule.step.val, numSteps, numContrSteps);
wrats = (wrats*M)./(ones(numWells, 1)*sum(M));
orats = (orats*M)./(ones(numWells, 1)*sum(M));
grats = (grats*M)./(ones(numWells, 1)*sum(M));
bhps  = (bhps*M)./(ones(numWells, 1)*sum(M));

% write output
fid = fopen(outNm, 'w');
fprintf(fid, '%+12.6e %+12.6e %+12.6e %+12.6e\n',  full([wrats(:) orats(:) grats(:) bhps(:)]'));
fclose(fid);

% Compute gradients and print to file
sens = runAdjointWellSensitivities(G, rock, fluid, schedule, system, 'ForwardStates', states);
W = processWellsLocal(G, rock, schedule.control(1));
writeWellSensitivities(W, sens, gradNm);

end

function grad = runAdjointWellSensitivities(G, rock, fluid, schedule, system, varargin) 

   directory = fullfile(fileparts(mfilename('fullpath')), 'cache');

   opt = struct('Verbose',             mrstVerbose, ...
                'writeOutput',         false, ...
                'directory',           directory, ...
                'outputName',          'adjoint', ...
                'outputNameFunc',      [], ...
                'simOutputName',       'state', ...
                'simOutputNameFunc',   [], ...
                'ForwardStates',       [], ...
                'ControlVariables',    [],...
                'scaling',             []);

   opt = merge_options(opt, varargin{:});

   vb = opt.Verbose;
   states = opt.ForwardStates;

   if ~isempty(opt.scaling)
      scalFacs = opt.scaling;
   else
      scalFacs.rate = 1; scalFacs.pressure = 1;
   end

   %--------------------------------------------------------------------------
   dts = schedule.step.val;
   tm = cumsum(dts);
   dispif(vb, '*****************************************************************\n')
   dispif(vb, '**** Starting adjoint simulation: %5.0f steps, %5.0f days *******\n', numel(dts), tm(end)/day)
   dispif(vb, '*****************************************************************\n')
   %--------------------------------------------------------------------------

   if opt.writeOutput
      % delete existing output
      % delete([opt.outputName, '*.mat']);
      % output file-names
      if isempty(opt.outputNameFunc)
         outNm  = @(tstep)fullfile(opt.directory, [opt.outputName, sprintf('%05.0f', tstep)]);
      else
         outNm  = @(tstep)fullfile(opt.directory, opt.outputNameFunc(tstep));
      end
   end
   if isempty(opt.simOutputNameFunc)
      inNm  = @(tstep)fullfile(opt.directory, [opt.simOutputName, sprintf('%05.0f', tstep)]);
   else
      inNm  = @(tstep)fullfile(opt.directory, opt.simOutputNameFunc(tstep));
   end

   nsteps = numel(dts);

   prevControl = inf;
   adjVec    = [];
   eqs_p       = [];
   gradFull = cell(1, numel(dts));

   % Load state - either from passed states in memory or on disk
   % representation.
   state = loadState(states, inNm, numel(dts));

   %-----------------------------------------------------------------------
   numControlSteps = numel(schedule.control);
   numWells        = [];

   timero = tic;
   for tstep = numel(dts):-1:1
       dispif(vb, 'Time step: %5.0f\n', tstep); timeri = tic;
       control = schedule.step.control(tstep);
       if control~=prevControl
           if (control==0)
               W = processWellsLocal(G, rock, [], 'createDefaultWell', true);
           else
           W = processWellsLocal(G, rock, schedule.control(control), 'Verbose', opt.Verbose, ...
                             'DepthReorder', true);
           if isempty(numWells) %initialize
               numWells = numel(W); %assum numWells is equa for all cont steps
               %if isfield(state.wellSol(1), 'qGs')
               %    numWellProps = 4;
               %else
                   numWellProps = 3;
               %end
               numRHS  = numControlSteps*numWells*numWellProps;
               %numVars = 2*numel(state.pressure) + numWells*numWellProps;
           end
           end
       end

       state_m = loadState(states, inNm, tstep-1);
       eqs = system.getEquations(state_m, state  , dts(tstep), G, W, system.s, fluid);
       numVars = cellfun(@numval, eqs)';
       cumVars = cumsum(numVars);
       ii = [[1;cumVars(1:end-1)+1], cumVars];
       eqs = cat(eqs{:});
       %compute weigth dt/dt_control
       steps  = schedule.step.control==control;
       weight = dts(tstep)/sum(dts(steps));
       rhs = getRHS1(control, [sum(numVars), numRHS], numWells, numWellProps, weight);
       
       
       if tstep < nsteps
           eqs_p = system.getEquations(state  , state_p, dts(tstep+1), G, W_p, system.s, fluid, ...
               'reverseMode', true);
           eqs_p = cat(eqs_p{:});
           rhs = rhs - eqs_p.jac{1}'*adjVec;
       end

       adjVec = eqs.jac{1}'\rhs;

       if isempty(opt.ControlVariables)
           gradFull{tstep} = -adjVec(ii(end,1):ii(end,2),:);
       else
           gradFull{tstep} = -adjVec(mcolon(ii(opt.ControlVariables,1),ii(opt.ControlVariables,2)),:);
       end

       state   = state_m;
       state_p = state;
       W_p = W;
       prevControl = control;
       dispif(vb, 'Done %6.2f\n', toc(timeri))
       if opt.writeOutput
           save(outNm(tstep), 'adjVec');
       end
   end

   grad = cell(1, numel(schedule.control));
   %take gradient for controll-step as sum of time-steps
   for k = 1:numel(schedule.control)
       ck = find((schedule.step.control == k));
       for m = 1:numel(ck)
       %subdts = dts(ck);
           if m == 1
               grad{k} = gradFull{ck(1)};
           else
               grad{k} = grad{k} + gradFull{ck(m)};
           end
       end
   end

   dispif(vb, '************Simulation done: %7.2f seconds ********************\n', toc(timero))
end
%--------------------------------------------------------------------------

function state = loadState(states, inNm, tstep)
if ~isempty(states)
    % State has been provided, no need to look for it.
    state = states{tstep+1};
    return
end
out = load(inNm(tstep));
fn  = fieldnames(out);
if numel(fn) ~= 1
    error('Unexpected format for simulation output')
else
    state = out.(fn{:});
end
end

function rhs = getRHS1(cStep, sz, nWells, nWQ, weight)
j1 = (cStep-1)*nWells*nWQ +1;
j2 = cStep*nWells*nWQ;
i1 = sz(1)-nWells*nWQ +1;
i2 = sz(1);
rhs = sparse(i1:i2, j1:j2, -weight, sz(1), sz(2));
end

function [] = writeWellSensitivities(W, sens, fnm)
fid = fopen(fnm, 'w');

nw = numel(W);
ns = numel(sens);
numPh = size(sens{1}, 2)/(nw*ns)-1;
%index mappings
if numPh == 2
    ix1 = repmat([1 3 4]', [1, ns*nw]);
    ix1 = ix1 + 4*ones(3,1)*(0:(nw*ns-1));
    ix1 = ix1(:);
    ix2 = repmat([nw+1, 1, 2*nw+1]', [1, nw]);
    ix2 = ix2 + ones(3,1)*(0:(nw-1));
    ix2 = ix2(:);
    ix2 = repmat(ix2, [1, ns]);
    ix2 = ix2 + 3*nw*ones(3*nw,1)*(0:(ns-1));
    ix2 = ix2(:);
end

%first, output well-names
for k = 1:nw
    fprintf(fid, [W(k).name, '\n']);
end

%loop over control steps
fmt = '%+12.6e ';
lft = [repmat(fmt, [1, 4*nw*ns]), '\n'];
MG  = zeros(nw, 4*nw*ns);
for cstep = 1:ns
    fprintf(fid, '\nSTEP %2.0d\n', cstep);
    M = full(sens{cstep});
    MG(:,ix1) = M(:, ix2);
    fprintf(fid, lft, MG');
end
fprintf(fid, '\n');
fclose(fid);
end
