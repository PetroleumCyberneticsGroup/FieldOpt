mrstPath = '/home/aleksaju/Work/postdoc/MRST/mrst-2013a';
run(fullfile(mrstPath,'startup.m'))

% change controls
i1 = [177.525, 142.661, 160.683, 183.471, 150.322]*meter^3/day;
i2 = [130, 130, 130, 130, 130]*meter^3/day;
p1 = [299.623, 313.594, 281.519, 274.477, 277.563]*barsa;
p2 = [313.616, 328.141, 314.562, 317.595, 327.316]*barsa;
p3 = [294.938, 310.44, 277.766, 267.372, 275.162]*barsa;
p4 = [291.015, 314.829, 280.442, 271.119, 283.161]*barsa;
p5 = [0, 0, 0, 0, 0]*meter^3/day;
u  = [i1; i2; p1; p2; p3; p4; p5];

try
 %write control file:
 fid = fopen('SIMPLE10x5x10_CONTROLS.TXT', 'w');
 fprintf(fid, '%+12.6e %+12.6e %+12.6e %+12.6e %+12.6e %+12.6e %+12.6e\n',  [i1; i2; p1; p2; p3; p4; p5]);
 fclose(fid);

 runSim2;
catch err
 warning('something went wrong with MRST');
 exit(3)
end
