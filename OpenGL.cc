#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#define Pi 3.141592654

typedef struct {
    int x;
    int y;
	double angle;
}Point;
int number=0;//combien de points sur ecran
int t;//combien de points sur enveloppe convexe
int ww=500,wh=500;//largeur et hauteur de fenetre 
Point points[50];
int stack[50]; //depot

void QSort(int l,int r);


void Swap(int i,int j)
{
	Point temp;
	temp=points[i];
	points[i]=points[j];
	points[j]=temp;
}

int Multi(Point p0, Point p1, Point p2)
{
    int k;
    k = (p1.x-p0.x)*(p2.y-p1.y) - (p2.x-p1.x)*(p1.y-p0.y);	  
    if(k < 0) 
	{

		return 1;
	}
    return 0; 
}

double Angle(int i)
{
    double j, k, m, n;
    j = points[i].x - points[0].x;
    k = fabs(points[i].y - points[0].y);
    m = sqrt(j*j+k*k);    
    n = acos(j/m)*180/Pi;
    return n;
}

void QSort(int l,int r)
{
	int i=l,j=r+1;
	double pivot=points[l].angle;
	Point temp=points[l];

	if(l>=r)return;
	
	
	while(1)
	{
		do
		{
			i=i+1;
		}while(i<=r&&points[i].angle<pivot);
		do
		{
			j=j-1;
		}while(j>=l&&points[j].angle>pivot);
		if(i>=j)break;
		Swap(i,j);
	}
	points[l]=points[j];
	points[j]=temp;
	QSort(l,j-1);
	QSort(j+1,r);

}

void convex()
{
	if(number>0)
	{
		int min=10000;
		int i,j=0;
		//premier etap
		for(i = 0; i < number; i++){     //trouver le premier point pour commencer l'algorithme Greham
			if(points[i].y < min){
				j = i;
				min = points[i].y;
			}
			else if(points[i].y == min && points[i].x < points[j].x) 
			{
				j = i;
			}
		}

		//deuxieme etap
		Swap(0, j); //On va regarder le point qui a le minimum de y comme le premier point

		//troisieme etap
		for(i = 1; i < number; i++){   //On va relier les autres points avec le premier point par les segments et on va calculer les angles entre ces segments et l'axe de X     
			points[i].angle = Angle(i);
		}

		//quatrieme etap
		QSort(1, number-1);  // Selon des angles du plus petit vers plus grand, on va trier les points


		//cinqieme etap
		for(i = 0; i <= 2; i++)
		{
			stack[i] = i;    //On va mettre 3 points dans le depot
		}
		t = 2;
		for(i=3;i<number;i++) 
		{
			//Si le nouveau point et les deux points qui sont plus haut de depot constituent un angle(pas saillie), on va tirer le point qui est plus haut de depot et on va recommecer faire la meme chose jusqu'a il n'y a plus de points dans le depot
			while(Multi(points[stack[t-1]], points[stack[t]], points[i])==1&&t>1) 
			{
				
				t--; 
				
			}	
			stack[++t] = i; // ajouter le nouveau point dans le depot
		}
	}
}


void myinit(void)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity(); 
	glViewport(0,0,ww,wh);
	glOrtho(0.0, (GLdouble) ww, 0.0, (GLdouble) wh, -1.0, 1.0);	
	glMatrixMode(GL_MODELVIEW);
    glClearColor (0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_COLOR_LOGIC_OP);
	glPointSize(5.0);
	glFlush();
	
}

void reshape(int w,int h)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, (GLdouble)w, 0.0, (GLdouble)h, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glViewport(0,0,w,h);
    glClear(GL_COLOR_BUFFER_BIT);
	glutPostRedisplay();
    glFlush();

    ww = w;
    wh = h; 
}

void display()
{
	int i;
	glClear(GL_COLOR_BUFFER_BIT);
	glBegin(GL_POINTS);
	for(i=0;i<number;i++)
	{
		glColor3ub( (char) rand()%256, (char) rand()%256, (char) rand()%256);
		glVertex2d(points[i].x,points[i].y);
	}
	glEnd();

	glColor3ub(0,255,0);
	glBegin(GL_LINE_LOOP);
	for(i=0;i<=t;i++)
		glVertex2d(points[stack[i]].x,points[stack[i]].y);
	glEnd();
	glFlush();
}


void mouse(int button,int state,int x,int y)
{
	y=wh-y;//conversion entre les systemes de cordonnee de Win fenetre et OpenGL mode cordonnee
	if(button==GLUT_LEFT_BUTTON&&state==GLUT_DOWN)
	{
		glColor3ub( (char) rand()%256, (char) rand()%256, (char) rand()%256);
		
		glBegin(GL_POINTS);
			glVertex2d(x,y);
		glEnd();
		glFlush();
		points[number].x=x;
		points[number++].y=y;
	}
}

void myMenu(int index)
{
	int i;
	switch(index)
	{
		case (1)://tracer le segment
		{		
			//Avant produire un nouveau enveloppe convexe, il faut annuler le segment ancien
			glColor3ub(0,255,0);
			//Tracer le nouveau segment sur le segment ancien
			glLogicOp(GL_XOR);//(00000000,11111111,00000000)<+>(00000000,11111111,00000000)=(00000000,00000000,00000000)
			glBegin(GL_LINE_LOOP);
			for(i=0;i<=t;i++)
				glVertex2d(points[stack[i]].x,points[stack[i]].y);
			glEnd();
			glFlush();
			
			convex();

			glBegin(GL_LINE_LOOP);
			for(i=0;i<=t;i++)
				glVertex2d(points[stack[i]].x,points[stack[i]].y);
			glEnd();
			glFlush();
			break;

		}
		case (2)://retouner au ecran d'original
		{
			number=0;
			t=0;
			glClear(GL_COLOR_BUFFER_BIT);
			glutPostRedisplay();
			break;

		}
		case (3)://creer automatiquement
		{
			int i=0;
			//annuler l'ecran
			number=0;
			t=0;
			glClear(GL_COLOR_BUFFER_BIT);
			glutPostRedisplay();
			//creer les points automatiquement, apres obtenir l'enveloppe convexe
			number=rand()%50;
			printf("creer %d points automatiquement\n",number);
			for(;i<number;i++)
			{
				points[i].x=rand()%ww;
				points[i].y=rand()%wh;
			}
			convex();
			glClear(GL_COLOR_BUFFER_BIT);
			glutPostRedisplay();
			break;

		}
	}
}

int main(int argc, char** argv)
{
	
	int menu =0;
	glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(ww, wh);
	glutInitWindowPosition(300, 150);
    glutCreateWindow("Enveloppe convexe");


	myinit ();

    glutDisplayFunc(display);
	glutReshapeFunc(reshape);

	menu = glutCreateMenu(myMenu);
    glutAddMenuEntry("trace_segments",1);
    glutAddMenuEntry("retourne_ecran_d'original",2);
	glutAddMenuEntry("cree_automatiquement",3);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	
    glutMouseFunc (mouse);
    glutMainLoop();

	return 0;
}

