// Parametres du modele d'Aliev-Paniflov
a=0.2; b=0.1; kk=8.0; M1= 0.07; M2=0.3; epsilon=0.01; d=5e-5;

// Definition des parametres d'execution
nx = 200; // Nombre de points de discretisation dans la direction x 
ny = 200; // Nombre de points de discretisation dans la direction y 

lx = 1.0; // Taille  de la boite dans la direction x
ly = 1.0; // Taille  de la boite dans la direction y

T = 1000.0; // Temps final

// Initialise les inconnues du probleme
E = zeros(nx,ny);
R = zeros(nx,ny);

// Sauvegarde du champs entre 2 iterations du schema temporel
E_prec = zeros(nx,ny);


// Pas d'espace 
dx = lx/(nx);

x = dx/2.:dx:lx;
X = repmat(x,nx,1);
Y = repmat(x',1,nx);

// Definie une condition initiale
for i = 1:nx,
  for j = 1:nx,
    if((dx*i-0.5)*(dx*i-0.5)+(dx*j-0.5)*(dx*j-0.5) < 0.005) then
      E(i,j) = 1.0;
    end
  end
end
//R(1:nx, ny/2:ny) = 1.;
E_prec = E;

// Contrainte pour le pas de temps
rp = kk*(b+1)*(b+1)/4.;
dte = (dx*dx)/(d*4+(dx*dx)*(rp+kk));
dtr = 1.0/(epsilon + (M1/M2) *rp);

// Pas de temps
dt = 0.95*min(dte,dtr);

// Coefficient multuplicateur du schema difference finie
alpha = d*dt/(dx*dx);

Ex=sparse([2:nx;1:nx-1]',ones(nx-1,1)',[nx,nx]);

Ax=Ex+Ex'- 2*speye(nx,nx); 
Ax(1,1) = -1; Ax(nx,nx) = -1;  // Conditions de Neumann
Ey=sparse([2:ny;1:ny-1]',ones(ny-1,1)',[ny,ny]);
Ay=Ey+Ey'-2*speye(ny,ny);
Ay(1,1) = -1; Ay(ny,ny) = -1; // Conditions de Neumann
A=speye(nx*ny,nx*ny) + alpha*((Ay .*. speye(nx,nx))+(speye(ny,ny) .*. Ax));
C = ((Ay .*. speye(nx,nx))+(speye(ny,ny) .*. Ax))/(dx*dx);
// Instant initial
t = 0.;
R = R(:);
i = 0;
k = 0;
// Resolution du systeme d'equation
while (t<T)
//while (t<2*dt)
  t = t + dt;
  i  = i+1;

  // Applique l'equation du champ electrique 
  E = A*E_prec(:);

  // Resout les equations explicites 
  E = E - dt*(kk*E .*  (E-a) .* (E-1) + E .*R);
  R = R - dt*(epsilon + M1 *R ./ (E  + M2)) .* (R+kk*E .* (E-b-1));

  // Mise à jour du champ 
  E_prec = E;

  if(modulo(i,10)==0)
     fprintfMat("Model2D/E_" + string(k) + ".dat", matrix(E,nx,ny), "%5.2f");
     fprintfMat("Model2D/R_" + string(k) + ".dat", matrix(R,nx,ny), "%5.2f");
    //if(k<10) then
      //fprintfMat("Model2D/E_0000" + string(k) + ".dat", matrix(E,nx,ny), "%5.2f");
      //fprintfMat("Model2D/R_0000" + string(k) + ".dat", matrix(R,nx,ny), "%5.2f");
    //elseif (k<100) then 
      //fprintfMat("Model2D/E_000" + string(k) + ".dat", matrix(E,nx,ny), "%5.2f");
      //fprintfMat("Model2D/R_000" + string(k) + ".dat", matrix(R,nx,ny), "%5.2f");
    //elseif (k<1000) then 
      //fprintfMat("Model2D/E_00" + string(k) + ".dat", matrix(E,nx,ny), "%5.2f");
      //fprintfMat("Model2D/R_00" + string(k) + ".dat", matrix(R,nx,ny), "%5.2f");
    //elseif (k<10000) then 
      //fprintfMat("Model2D/E_0" + string(k) + ".dat", matrix(E,nx,ny), "%5.2f");
      //fprintfMat("Model2D/R_0" + string(k) + ".dat", matrix(R,nx,ny), "%5.2f");
    //else 
      //fprintfMat("Model2D/E_" + string(k) + ".dat", matrix(E,nx,ny), "%5.2f");
      //fprintfMat("Model2D/R_" + string(k) + ".dat", matrix(R,nx,ny), "%5.2f");
    //end
    k = k + 1;
  end

end

//E = matrix(E,nx,ny);
//R = matrix(R,nx,ny);

//surf(E,'interp')
//f=gcf();
//f.color_map = jetcolormap(256);
//a = gca();
//a.view = "2d";
