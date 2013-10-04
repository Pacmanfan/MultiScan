
// I N C L U D E S ///////////////////////////////////////////////////////////

#include "math3d.h"     // contains header info about points and matrices
#include "log.h"
#include "vector3d.hpp" //this is the 3d vector system.

// look up tables

float sinlook[numentrys],coslook[numentrys];
bool tablesbuilt = false;
void Build_Look_Up_Tables(void)
{
	Log("building lookup tables");
	long double unit = (long double)(3.14159 * 2.0f ) / (long double) numentrys;
	for (unsigned short  i = 0 ; i < numentrys ; i++){
                long double Degree = (long double)i;
		coslook[i] = float(cos(unit * Degree));
		sinlook[i] = float(sin(unit * Degree));
	}
	tablesbuilt = true;
} // end Build_Look_Up_Tables
//float SIN(float n){return sinlook[int(n*stepprecfact)];}
//float COS(float n){return coslook[int(n*stepprecfact)];}


// Function designed to set matrix to identity matrix:
void Matrix3D::Initialize ()
   {
   Matrix[0][0] = 1;  Matrix[0][1] = 0;  Matrix[0][2] = 0;  Matrix[0][3] = 0;
   Matrix[1][0] = 0;  Matrix[1][1] = 1;  Matrix[1][2] = 0;  Matrix[1][3] = 0;
   Matrix[2][0] = 0;  Matrix[2][1] = 0;  Matrix[2][2] = 1;  Matrix[2][3] = 0;
   Matrix[3][0] = 0;  Matrix[3][1] = 0;  Matrix[3][2] = 0;  Matrix[3][3] = 1;
   }
void Matrix3D::Print (){
	Log("Matrix:");
	for(int i=0;i<4;i++){
		Log("%f,%f,%f,%f",Matrix [i][0],Matrix [i][1],Matrix [i][2],Matrix [i][3]);
	}
}
void Matrix3D::InitMat ( float Mat [ 4 ] [ 4 ] )
   {
   // Initializes a specific matrix to the identity matrix:
   Mat [0][0] = 1;  Mat [0][1] = 0;  Mat [0][2] = 0;  Mat [0][3] = 0;
   Mat [1][0] = 0;  Mat [1][1] = 1;  Mat [1][2] = 0;  Mat [1][3] = 0;
   Mat [2][0] = 0;  Mat [2][1] = 0;  Mat [2][2] = 1;  Mat [2][3] = 0;
   Mat [3][0] = 0;  Mat [3][1] = 0;  Mat [3][2] = 0;  Mat [3][3] = 1;   
   }
void Matrix3D::MergeMatrix ( float NewMatrix [ 4 ] [ 4 ] )
   {
   // Multiply NewMatirx by Matrix; store result in TempMatrix
   float TempMatrix [ 4 ] [ 4 ];
	for (short unsigned int i = 0; i < 4; i++)
		 for (short unsigned int j = 0; j < 4; j++) 
			  TempMatrix[i][j] = (Matrix[i][0] * NewMatrix[0][j])
                            + (Matrix[i][1] * NewMatrix[1][j])
                            + (Matrix[i][2] * NewMatrix[2][j])
                            + (Matrix[i][3] * NewMatrix[3][j]);
   // Copy TempMatrix to Matrix:
   for (int i = 0; i < 4; i++)
       {
       Matrix[i][0] = TempMatrix[i][0];
       Matrix[i][1] = TempMatrix[i][1];
       Matrix[i][2] = TempMatrix[i][2];
       Matrix[i][3] = TempMatrix[i][3];
       }   
   }

void Matrix3D::MergeMatrices ( float Dest [ 4 ] [ 4 ], float Source [ 4 ] [ 4 ] )
   {
   // Multiply Source by Dest; store result in Temp:
   float Temp [ 4 ] [ 4 ];
	for ( short unsigned int i = 0; i < 4; i++ )
		 for ( short unsigned int j = 0; j < 4; j++ )
           {
			  Temp [ i ] [ j ] = ( Source [ i ] [ 0 ] * Dest [ 0 ] [ j ] )
                            + ( Source [ i ] [ 1 ] * Dest [ 1 ] [ j ] )
                            + ( Source [ i ] [ 2 ] * Dest [ 2 ] [ j ] )
                            + ( Source [ i ] [ 3 ] * Dest [ 3 ] [ j ] );
           }
   // Copy Temp to Dest:
   for (int i = 0; i < 4; i++)
       {
       Dest [ i ] [ 0 ] = Temp [ i ] [ 0 ];
       Dest [ i ] [ 1 ] = Temp [ i ] [ 1 ];
       Dest [ i ] [ 2 ] = Temp [ i ] [ 2 ];
       Dest [ i ] [ 3 ] = Temp [ i ] [ 3 ];
       }

   }
   
void  Matrix3D::Rotate ( float Xa, float Ya, float Za )
   {
   // Generate 3D rotation matrix:
  // Xr = QuickFist(Xa); Yr = QuickFist(Ya); Zr = QuickFist(Za);
   float Rmat [ 4 ] [ 4 ];
   float sinxa = SIN(Xa),cosxa=COS(Xa),sinza= SIN(Za),cosza=COS(Za),
	   sinya= SIN(Ya),cosya=COS(Ya); 
   InitMat ( RMatrix );

   // Initialize Z rotation matrix - Note: we perform Z
   // rotation first to align the 3D Z axis with the 2D Z axis.
   Rmat[0][0]=cosza;  Rmat[0][1]=sinza;  Rmat[0][2]=0;    Rmat[0][3]=0;
   Rmat[1][0]=-sinza; Rmat[1][1]=cosza;  Rmat[1][2]=0;    Rmat[1][3]=0;
   Rmat[2][0]=0;        Rmat[2][1]=0;        Rmat[2][2]=1;    Rmat[2][3]=0;
   Rmat[3][0]=0;        Rmat[3][1]=0;        Rmat[3][2]=0;    Rmat[3][3]=1;

   // Merge matrix with master matrix:
   MergeMatrices ( RMatrix, Rmat );

   // Initialize X rotation matrix:
   Rmat[0][0]=1;  Rmat[0][1]=0;        Rmat[0][2]=0;       Rmat[0][3]=0;
   Rmat[1][0]=0;  Rmat[1][1]=cosxa;  Rmat[1][2]=sinxa; Rmat[1][3]=0;
   Rmat[2][0]=0;  Rmat[2][1]=-sinxa; Rmat[2][2]=cosxa; Rmat[2][3]=0;
   Rmat[3][0]=0;  Rmat[3][1]=0;        Rmat[3][2]=0;       Rmat[3][3]=1;

   // Merge matrix with master matrix:
   MergeMatrices ( RMatrix, Rmat );

   // Initialize Y rotation matrix:
   Rmat[0][0]=cosya; Rmat[0][1]=0;   Rmat[0][2]=-sinya; Rmat[0][3]=0;
   Rmat[1][0]=0;       Rmat[1][1]=1;   Rmat[1][2]=0;        Rmat[1][3]=0;
   Rmat[2][0]=sinya; Rmat[2][1]=0;   Rmat[2][2]=cosya;  Rmat[2][3]=0;
   Rmat[3][0]=0;       Rmat[3][1]=0;   Rmat[3][2]=0;        Rmat[3][3]=1;

   // Merge matrix with master matrix:
   MergeMatrices ( RMatrix, Rmat );

   MergeMatrix ( RMatrix );
   ;
   }
void Matrix3D::Set(float x,float y,float z,short row){
	Matrix[row][0] = x;
	Matrix[row][1] = y;
	Matrix[row][2] = z;
}   
void Matrix3D::SetRow(float x,float y,float z,short row){
	Matrix[0][row] = x;
	Matrix[1][row] = y;
	Matrix[2][row] = z;
}   
void Matrix3D::Get(float *x,float *y,float *z,short row){
	 *x = Matrix[row][0];
	 *y = Matrix[row][1];
	 *z = Matrix[row][2];
}   
void Matrix3D::GetRow(float *x,float *y,float *z,short col){
	 *x = Matrix[0][col];
	 *y = Matrix[1][col];
	 *z = Matrix[2][col];
}   
void Matrix3D::Translate ( float Xt, float Yt, float Zt )
   {
   // Create 3D translation matrix:

   // Declare translation matrix:
   float Tmat [ 4 ] [ 4 ];
   
   // Save translation values:
  // XTrans = Xt; YTrans = Yt; ZTrans = Zt;
   
   // Initialize translation matrix:
   Tmat[0][0]=1;  Tmat[0][1]=0;  Tmat[0][2]=0;  Tmat[0][3]=0;
   Tmat[1][0]=0;  Tmat[1][1]=1;  Tmat[1][2]=0;  Tmat[1][3]=0;
   Tmat[2][0]=0;  Tmat[2][1]=0;  Tmat[2][2]=1;  Tmat[2][3]=0;
   Tmat[3][0]=Xt; Tmat[3][1]=Yt; Tmat[3][2]=Zt; Tmat[3][3]=1;

   // Merge matrix with master matrix:
   MergeMatrix ( Tmat );
   
   }
   
// Function designed to merge scaling matrix with master
// matrix:
void  Matrix3D::Scale ( float Xs, float Ys, float Zs )
   {
   // Create 3D scaling matrix:
   float Smat [ 4 ] [ 4 ];

   // Initialize scaling matrix:
   Smat[0][0] = Xs; Smat[0][1] = 0;  Smat[0][2] = 0;  Smat[0][3] = 0;
   Smat[1][0] = 0;  Smat[1][1] = Ys; Smat[1][2] = 0;  Smat[1][3] = 0;
   Smat[2][0] = 0;  Smat[2][1] = 0;  Smat[2][2] = Zs; Smat[2][3] = 0;
   Smat[3][0] = 0;  Smat[3][1] = 0;  Smat[3][2] = 0;  Smat[3][3] = 1;

   // Merge matrix with master matrix:
   MergeMatrix ( Smat );
   }

void  Matrix3D::Shear ( float Xs, float Ys )
   {
   // Create 3D shearing matrix:

   float Smat [ 4 ] [ 4 ];

   // Initialize shearing matrix:
   Smat[0][0] = 1;  Smat[0][1] = 0;  Smat[0][2] = Xs;  Smat[0][3] = 0;
   Smat[1][0] = 0;  Smat[1][1] = 1;  Smat[1][2] = Ys;  Smat[1][3] = 0;
   Smat[2][0] = 0;  Smat[2][1] = 0;  Smat[2][2] = 1;   Smat[2][3] = 0;
   Smat[3][0] = 0;  Smat[3][1] = 0;  Smat[3][2] = 0;   Smat[3][3] = 1;

   // Merge matrix with master matrix:
   MergeMatrix ( Smat );
   }

void Matrix3D::Transform(point_3d &V){

	V.Cx = ( (   V.Wx * Matrix[0][0] ) )
          + ( ( V.Wy * Matrix [1][0] ) )
          + ( ( V.Wz * Matrix [ 2 ][ 0 ]) )
          + Matrix [ 3 ][ 0 ];
   V.Cy = (   ( V.Wx * Matrix [ 0 ][ 1 ]) )
          + ( ( V.Wy * Matrix [ 1 ][ 1 ]) )
          + ( ( V.Wz * Matrix [ 2 ][ 1 ]) )
          + Matrix [ 3 ][ 1 ];
   V.Cz = (   ( V.Wx * Matrix [ 0 ][ 2 ]) )
          + ( ( V.Wy * Matrix [ 1 ][ 2 ]) )
          + ( ( V.Wz * Matrix [ 2 ][ 2 ]) )
          + Matrix [ 3 ][ 2 ];
}

void Matrix3D::TransformWorld(point_3d &V){
   // Initialize temporary variables:
   static float Wx,Wy,Wz;
   Wx = V.Wx;
   Wy = V.Wy;
   Wz = V.Wz;
   V.Wx = ( (   Wx * Matrix [ 0 ][ 0 ]) )
          + ( ( Wy * Matrix [ 1 ][ 0 ]) )
          + ( ( Wz * Matrix [ 2 ][ 0 ]) )
          + Matrix [ 3 ][ 0 ];

   V.Wy = (   ( Wx * Matrix [ 0 ][ 1 ]) )
          + ( ( Wy * Matrix [ 1 ][ 1 ]) )
          + ( ( Wz * Matrix [ 2 ][ 1 ]) )
          + Matrix [ 3 ][ 1 ];

   V.Wz = (   ( Wx * Matrix [ 0 ][ 2 ]) )
          + ( ( Wy * Matrix [ 1 ][ 2 ]) )
          + ( ( Wz * Matrix [ 2 ][ 2 ]) )
          + Matrix [ 3 ][ 2 ];
   }

void Matrix3D::Inverse(Matrix3D &dest){// calc the inverse
   //float InvMatrix [ 4 ] [ 4 ];
   double Pivot;
	int i, j, k;

   for ( i = 0; i < 4; i++ )
       {
       dest.Matrix [ i ] [ 0 ] = Matrix [ i ] [ 0 ];
       dest.Matrix [ i ] [ 1 ] = Matrix [ i ] [ 1 ];
       dest.Matrix [ i ] [ 2 ] = Matrix [ i ] [ 2 ];
       dest.Matrix [ i ] [ 3 ] = Matrix [ i ] [ 3 ];
       }

   for ( i = 0; i < 4; i++ )
       {
       Pivot = dest.Matrix [ i ] [ i ];
       dest.Matrix [ i ] [ i ] = 1.0F;
       for ( j = 0; j < 4; j++)
           dest.Matrix [ i ] [ j ] /=(float)Pivot;
       for ( k = 0; k < 4; k++)
           {
           if ( k == i )
              continue;
           Pivot = dest.Matrix [ k ] [ i ];
           dest.Matrix [ k ] [ i ] = 0.0F;
           for ( j = 0; j < 4; j++ )
               dest.Matrix [ k ] [ j ] -=(float)( Pivot * dest.Matrix [ i ] [ j ]);
           }
       }	
}

point_3d &Matrix3D::Untransform ( point_3d &V )
   {
   // Initialize temporary variables:
   float Cx = V.Cx;
   float Cy = V.Cy;
   float Cz = V.Cz;
   float InvMatrix [ 4 ] [ 4 ];
   double Pivot;
	int i, j, k;

   for ( i = 0; i < 4; i++ )
       {
       InvMatrix [ i ] [ 0 ] = Matrix [ i ] [ 0 ];
       InvMatrix [ i ] [ 1 ] = Matrix [ i ] [ 1 ];
       InvMatrix [ i ] [ 2 ] = Matrix [ i ] [ 2 ];
       InvMatrix [ i ] [ 3 ] = Matrix [ i ] [ 3 ];
       }

   for ( i = 0; i < 4; i++ )
       {
       Pivot = InvMatrix [ i ] [ i ];
       InvMatrix [ i ] [ i ] = 1.0F;
       for ( j = 0; j < 4; j++)
           InvMatrix [ i ] [ j ] /=(float)Pivot;
       for ( k = 0; k < 4; k++)
           {
           if ( k == i )
              continue;
           Pivot = InvMatrix [ k ] [ i ];
           InvMatrix [ k ] [ i ] = 0.0F;
           for ( j = 0; j < 4; j++ )
               InvMatrix [ k ] [ j ] -=(float)( Pivot * InvMatrix [ i ] [ j ]);
           }
       }

   // Transform vertex by inverse master matrix:
   V.Wx = ( (   Cx * InvMatrix [ 0 ][ 0 ]) )
          + ( ( Cy * InvMatrix [ 1 ][ 0 ]) )
          + ( ( Cz * InvMatrix [ 2 ][ 0 ]) )
          +          InvMatrix [ 3 ][ 0 ];

   V.Wy = (   ( Cx * InvMatrix [ 0 ][ 1 ]) )
          + ( ( Cy * InvMatrix [ 1 ][ 1 ]) )
          + ( ( Cz * InvMatrix [ 2 ][ 1 ]) )
          +          InvMatrix [ 3 ][ 1 ];

   V.Wz = (   ( Cx * InvMatrix [ 0 ][ 2 ]) )
          + ( ( Cy * InvMatrix [ 1 ][ 2 ]) )
          + ( ( Cz * InvMatrix [ 2 ][ 2 ]) )
          +          InvMatrix [ 3 ][ 2 ];
   return V;
   }


// Function designed to transform a vector using the master
// matrix:

Vector3d &Matrix3D::Transform ( Vector3d &V )
   {
   // Initialize temporary variables:
   float OldX = V.x;
   float OldY = V.y;
   float OldZ = V.z;

   // Transform vertex by master matrix:
   V.Tx = ( ( OldX * Matrix [ 0 ] [ 0 ] ) )
        + ( ( OldY * Matrix [ 1 ] [ 0 ] ) )
        + ( ( OldZ * Matrix [ 2 ] [ 0 ] ) )
        +            Matrix [ 3 ] [ 0 ];

   V.Ty = ( ( OldX * Matrix [ 0 ] [ 1 ] ) )
        + ( ( OldY * Matrix [ 1 ] [ 1 ] ) )
        + ( ( OldZ * Matrix [ 2 ] [ 1 ] ) )
        +            Matrix [ 3 ] [ 1 ];

   V.Tz = ( ( OldX * Matrix [ 0 ] [ 2 ] ) )
        + ( ( OldY * Matrix [ 1 ] [ 2 ] ) )
        + ( ( OldZ * Matrix [ 2 ] [ 2 ] ) )
        +            Matrix [ 3 ] [ 2 ];
   return V;
   }

void Matrix3D::Load(FILE *fp){
    fread(&Matrix, 16 * sizeof(float),1,fp);
}
void Matrix3D::Save(FILE *fp){
	fwrite(&Matrix, 16 * sizeof(float),1,fp);
}

void Matrix3D::SetPosition(float x,float y,float z){
		 Matrix[3][0] = -x;
		 Matrix[3][1] = -y;
		 Matrix[3][2] = -z;
}
void Matrix3D::GetPosition(float &x,float &y, float &z){
		point_3d tmp,retval;
		tmp.Cx=0.0f;
		tmp.Cy=0.0f;
		tmp.Cz=0.0f;
		retval = Untransform(tmp);
		x = retval.Wx;
		y = retval.Wy;
		z = retval.Wz;
}

float Random(float n){return ((1/RAND_MAX*rand())*n);}
