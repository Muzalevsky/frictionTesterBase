close all;
dt = 0.00001;
S=5.5*261;
Vmax=200;
E=5000;
tnas=Vmax/E;
s_nas1=S-E*tnas^2;
t_usk = sqrt(s_nas1/E);
t1=0:dt:tnas;
v1=E*t1;
t2=tnas:dt:t_usk;
v2=zeros(1,length(t2))+Vmax;
t3=t_usk:dt:t_usk+tnas;
v3=E*(t_usk+tnas)-E*t3;
t=[t1 t2 t3];
v=[v1 v2 v3];
v
plot(t,v);
grid on