import py.codac4matlab.*

currentFilePath = mfilename('fullpath');
[currentDir, ~, ~] = fileparts(currentFilePath)
cd(currentDir);
% 2D example of the PEIBOS algorithm

y_2d=VectorVar(2);
a=1.4;
b=0.3;

f_2d = AnalyticFunction({y_2d},vec(y_2d(2)+1-a*sqr(y_2d(1)),b*y_2d(1)));

X_2d = VectorVar(1);
psi0_2d = AnalyticFunction({X_2d},vec(cos(X_2d(1)*PI/4.-PI/2),sin(X_2d(1)*PI/4.-PI/2)));

generators_2d = int64([1,2;-2,1]);

v_par_2d = PEIBOS(f_2d,psi0_2d,generators_2d,0.1);

output_2d = GraphicOutput(0x01);
figure_2d = Figure2D("Henon Map",output_2d);

for i = 1:length(v_par_2d)
    par = v_par_2d{i};
    figure_2d.draw_parallelepiped(par.z, par.A, StyleProperties({Color().green(),Color().green(0.5)}));
end

%  3D example of the PEIBOS algorithm

y_3d = VectorVar(3);
f_3d = AnalyticFunction({y_3d},vec(sqr(y_3d(1))-sqr(y_3d(2))+y_3d(1),2*y_3d(1)*y_3d(2)+y_3d(2),y_3d(3)));

X_3d = VectorVar(2);
psi0_3d = AnalyticFunction({X_3d},vec(1/sqrt(1+sqr(X_3d(1))+sqr(X_3d(2))),X_3d(1)/sqrt(1+sqr(X_3d(1))+sqr(X_3d(2))),X_3d(2)/sqrt(1+sqr(X_3d(1))+sqr(X_3d(2)))));

generators_3d = int64([1, 2, 3; -2, 1, 3; 3, 2, -1]);

v_par_3d = PEIBOS(f_3d,psi0_3d,generators_3d,0.2);

figure_3d = Figure3D("Conform");
figure_3d.draw_axes();

for i = 1:length(v_par_3d)
    par = v_par_3d{i};
    figure_3d.draw_parallelepiped(par.z, par.A, StyleProperties(Color().green(0.5)));
end