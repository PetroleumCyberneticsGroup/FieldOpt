function state = initStateADI(G, f, deck)
eql      = deck.SOLUTION.EQUIL;
datDepth = eql(1);
datPres  = eql(2);
woc      = eql(3);

g = norm(gravity);

% approx by linear pressure drop/linear bo from datDepth - woc
pwoc = ADI(datPres, 1);
nRes = inf;
rhoDat = f.rhoOS./f.BO(datPres);
while nRes/norm(pwoc.val) > 1e-12
    rho    = f.rhoOS./f.BO(pwoc);
    eq = pwoc - datPres - g*(woc-datDepth)*(rho+rhoDat)/2;
    dp = -eq.val./eq.jac{1};
    pwoc  = ADI(pwoc.val+dp, 1);
    nRes = norm(eq.val);
end

% approx by linear pressure drop/linear bw from woc - bottom
bottom = max(G.cells.centroids(:,3));
pbottom = ADI(pwoc.val, 1);
nRes = inf;
rhowoc = f.rhoWS./f.BW(pwoc.val);
while nRes/norm(pbottom.val) > 1e-12
    rho    = f.rhoWS./f.BW(pbottom);
    eq = pbottom - pwoc.val - g*(bottom-woc)*(rho+rhowoc)/2;
    dp = -eq.val./eq.jac{1};
    pbottom  = ADI(pbottom.val+dp, 1);
    nRes = norm(eq.val);
end

tab = [datDepth datPres; woc pwoc.val; bottom pbottom.val];
state.pressure = interp1q(tab(:,1), tab(:,2), G.cells.centroids(:,3));

%--------------------------------------------------------------------------

tab = deck.PROPS.SWOF{1};
maxs = tab(end,1);
mins = tab(1,1);

sw = ones(G.cells.num, 1)*mins;
sw(G.cells.centroids(:,3) > woc) = maxs;

state.s = [sw 1-sw];
state.flux = zeros(G.faces.num, 1);