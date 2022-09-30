#include <GL/freeglut.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#define radianoParaGraus(radianos) (radianos * (180.0 / M_PI))
#define grausParaRadianos(graus) ((graus * M_PI) / 180.0)
#define largura_m 6
#define altura_m 6
#define largura_aviao 6
#define altura_aviao 9.5
GLint figure;
GLfloat aspectRat;
GLint cons = 0, cons2 = 0;

GLfloat P1_orient_y = 100, M1_orient_y = 0.5;
GLfloat P2_orient_y = 60, M2_orient_y = 0.75;


typedef struct {
    float P_orient_x;
    float P_orient_y;
    float angulo;
    float velocidade;
    int colidindo;
    int future_use;
} pos;

typedef struct{
    int wPressed;
    int sPressed;
    int aPressed;
    int dPressed;
    int key_left;
    int key_right;
    int key_up;
    int key_down;
}keyboard_t;

keyboard_t trataTeclado;

pos lista[6];

void criaListaStruct(){

    pos player = {
        .P_orient_x = 0.0,
        .P_orient_y = 0.0,
        .angulo = 90.0,
        .velocidade = 2.0,
        .colidindo = 0,
    };

    pos aviao_1 = {
        .P_orient_x = 30.0,
        .P_orient_y = 100.0,
        .angulo = 0,
        .velocidade = 0.5,
        .colidindo = 0
    };

    pos aviao_2 = {
        .P_orient_x = -40.0,
        .P_orient_y = 100.0,
        .angulo = 0,
        .velocidade = 0.75,
        .colidindo = 0
    };

    pos nave_1 = {
        .P_orient_x = -60.0,
        .P_orient_y = -100,
        .velocidade = 0.8,
        .angulo = 0,
        .colidindo = 0
    };

    pos nave_2 = {
        .P_orient_x = 60.0,
        .P_orient_y = -100,
        .velocidade = 0.8,
        .angulo = 0,
        .colidindo = 0
    };

    pos missil = {
        .P_orient_x = 50.0,
        .P_orient_y = -50.0,
        .velocidade = 0,
        .angulo =0,
        .colidindo = 0
    };

    lista[0] = player;
    lista[1] = aviao_1;
    lista[2] = aviao_2;
    lista[3] = nave_1;
    lista[4] = nave_2;
    lista[5] = missil;
}

void estaTela(){
    if(lista[0].P_orient_y > 96 - altura_m )
        lista[0].P_orient_y = 96 - altura_m;
    if(lista[0].P_orient_y < -96 + altura_m)
        lista[0].P_orient_y = -96 + altura_m;
    if(lista[0].P_orient_x > (96 - largura_m) * aspectRat)
        lista[0].P_orient_x = (96 - largura_m) * aspectRat;
    if(lista[0].P_orient_x < (-96 + largura_m) * aspectRat)
        lista[0].P_orient_x = (-96 + largura_m) * aspectRat;
}

void atualiza_aviao1(){
    lista[1].P_orient_y -= lista[1].velocidade;
    glutPostRedisplay();
}

void atualiza_aviao2(){
    lista[2].P_orient_y -= lista[2].velocidade;
    glutPostRedisplay();
}

void atualiza_nave_1(){
    lista[3].P_orient_y += lista[3].velocidade;
    glutPostRedisplay();
}

void atualiza_nave_2(){
    lista[4].P_orient_y += lista[4].velocidade;
    glutPostRedisplay();
}

void verifica_crit(int cons, int objeto){
    if(cons == 0){
        if(objeto == 1)
            atualiza_aviao1();            
            if(lista[1].P_orient_y <= -100 * aspectRat + 9.5){
                cons = 1;
            }
        if(objeto == 2)
            atualiza_aviao2();
            if(lista[2].P_orient_y <= -100 * aspectRat + 9.5){
                cons = 1;
            }
        }
        if(objeto == 3){
            atualiza_nave_1();
            if(lista[3].P_orient_y >= 100 * aspectRat - 6){
                cons = 1;
            }
        }
        if(objeto == 4){
            atualiza_nave_2();
            if(lista[4].P_orient_y >= 100 * aspectRat - 6){
                cons = 1;
            }
        }

    if(cons == 1){
        if(objeto == 1){
            lista[1].P_orient_y = 80;
            atualiza_aviao1();
            cons=0;

        }
        if(objeto == 2){
            lista[2].P_orient_y = 100;
            atualiza_aviao2();
            cons=0;
        }

        if(objeto == 3){
            lista[3].P_orient_y = -100;
            atualiza_nave_1();
            cons=0;
        }

        if(objeto == 4){
            lista[4].P_orient_y = -100;
            atualiza_nave_2();
            cons=0;
        }

    }
}

void movementPlayer(){
    lista[0].angulo += ((trataTeclado.aPressed || trataTeclado.key_left) - (trataTeclado.dPressed || trataTeclado.key_right)) * 3;
    int movement = ((trataTeclado.wPressed || trataTeclado.key_up) - (trataTeclado.sPressed || trataTeclado.key_down));

    lista[0].P_orient_x += movement * lista[0].velocidade * cos(grausParaRadianos(lista[0].angulo));
    lista[0].P_orient_y += movement * lista[0].velocidade * sin(grausParaRadianos(lista[0].angulo));

}

void escreve(void* fonte, GLubyte* texto, GLfloat x, GLfloat y) {
    glColor3f(0.8,0.9, 0.85);
    glPushMatrix(); // Salve a condinção atual
        glLoadIdentity(); // Vá para a identidade inicial do programa
        glRasterPos2f(x-20, y);
        for (int i = 0; i < strlen(texto); i++) {
            glutBitmapCharacter(fonte, texto[i]);
        }
    glPopMatrix(); // Volta para a anterior, onde está o push
}

bool intersec1(GLfloat aPosition_x, GLfloat aPosition_y, GLfloat aLarg, GLfloat aAlt, GLfloat bPosition_x, GLfloat bPosition_y, GLfloat bLarg, GLfloat bAlt){
    /*Analisando se apesar de estar no mesmo x, os valores de y são diferentes (Verif. lado esquerdo)  
                                                    A
                                                B                                                 */  
    if((aPosition_x  - aLarg == bPosition_x + bLarg) && (aPosition_y != bPosition_y)){return false;}

    /*Analisando se apesar de estar no mesmo x, os valores de y são diferentes (Verif. lado esquerdo)  
                                            A
                                                B                                                   */
    if((aPosition_x + aLarg == bPosition_x - bLarg) && (aPosition_y != bPosition_y)){return false;}

    // A B
    if(aPosition_x - aLarg > bPosition_x + bLarg){return false;}

    // B A
    if(aPosition_x + aLarg < bPosition_x - bLarg){return false;}

    /*Analisando parte de baixo B
                                A  */
    if(aPosition_y + aAlt < bPosition_y - bAlt){return false;}

    /* Analisando parte de cima A
                                B   */
    if(aPosition_y - aAlt > bPosition_y + bAlt){return false;}

    lista[0].colidindo = 1;
    lista[0].P_orient_x = 0;
    lista[0].P_orient_y = 0;
    lista[0].angulo = 90;
    lista[1].P_orient_y = 100;
    lista[1].colidindo = 1;
    lista[2].P_orient_y = 80;
    lista[2].colidindo = 1;
    lista[3].P_orient_y = -100;
    lista[3].colidindo = 1;
    lista[4].P_orient_y = -100;
    lista[4].colidindo = 1;
    lista[5].P_orient_x = 50;
    lista[5].P_orient_y = -50;
    lista[5].colidindo = 1;

    glClear(GL_COLOR_BUFFER_BIT);
    escreve(GLUT_BITMAP_HELVETICA_18, "CAPTURED", 0, 0);
    glutSwapBuffers();
    sleep(1);

    return true;
}

void aviao(){
    //LARGURA TOTAL atual - 6 -> -6 = 12 Largura
    //ALtura total atual - 10 -> -2 = 12 Tamanho

    //Ponta Esquerda Milenium
    glBegin(GL_TRIANGLE_FAN);
        glColor3f(1,1,1);
        glVertex3f(-1,10,0);
        glVertex3f(-3,8,0);
        glVertex3f(-4,5,0);
        glVertex3f(-1,7,0);
    glEnd();

    glColor3f(0,0,0);
    //Ligação entre as pontas
    glBegin(GL_TRIANGLE_FAN);
        glColor3ub(75, 0, 130);
        glVertex3f(-4,5,0);
        glVertex3f(4,5,0);
        glVertex3f(1,7,0);
        glVertex3f(-1,7,0);
    glEnd();  

    //Ponta direita
    glBegin(GL_TRIANGLE_FAN);
        glColor3f(1,1,1);
        glVertex3f(1,10,0);
        glVertex3f(3,8,0);
        glVertex3f(4,5,0);
        glVertex3f(1,7,0);
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
        glColor3ub(75, 0, 130);
        glVertex3f(0,-2,0);
        glVertex3f(0,5,0);
        glVertex3f(-4,5,0);
        glVertex3f(4,5,0);
    glEnd();


    glBegin(GL_TRIANGLE_FAN);
        glColor3ub(75, 0, 130);
        glVertex3f(0,-2,0);
        glVertex3f(0,5,0);
        glVertex3f(4,5,0);
        glVertex3f(-4,5,0);
    glEnd();

    glColor3f(0,0,0);
    glBegin(GL_TRIANGLE_FAN);
        glVertex3f(0,-2,0);
        glVertex3f(5,-1,0);
        glVertex3f(6,2,0);
        glVertex3f(3,-2,0);
        glColor3f(1,0,0);
        glVertex3f(5,6,0);
        glVertex3f(3,-2,0);
    glEnd();

    glColor3f(0,0,0);
    glBegin(GL_TRIANGLE_FAN);
        glVertex3f(0,-2,0);
        glVertex3f(-5,-1,0);
        glVertex3f(-6,2,0);
        glVertex3f(-3,-2,0);
        glColor3f(1,0,0);
        glVertex3f(-5,6,0);
        glVertex3f(-3,-2,0);
    glEnd();
}

void missil_segue_player(){
    glColor3f(1, 0, 0.5f);
    glBegin(GL_TRIANGLE_FAN);
        glVertex3f(0, 2, 0);
        glVertex3f(1, 0, 0);
        glVertex3f(0, -2, 0);
        glVertex3f(-1, 0, 0);
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
        glVertex3f(1,2,0);
        glVertex3f(1.5,2,0);
        glVertex3f(1.5,-2,0);
        glVertex3f(1,-2, 0);
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
        glVertex3f(-1,2,0);
        glVertex3f(-1.5,2,0);
        glVertex3f(-1.5,-2,0);
        glVertex3f(-1,-2, 0);
    glEnd();
}

void nave_1(){
    glColor3f(0.2, 0.7,0.1);
    glBegin(GL_TRIANGLE_FAN);
        glVertex3f(0, 6, 0);
        glVertex3f(1, 4, 0);
        glVertex3f(-1, 4, 0);
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
        glVertex3f(-1,4,0);
        glVertex3f(-1,2,0);
        glVertex3f(1, 2, 0);
        glVertex3f(1, 4, 0);
    glEnd();
        
    glBegin(GL_TRIANGLE_FAN);
        glVertex3f(-1,4,0);
        glVertex3f(-3,0,0);
        glVertex3f(-1, 2, 0);
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
        glVertex3f(1,4,0);
        glVertex3f(3,0,0);
        glVertex3f(1, 2, 0);
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
        glVertex3f(-1, 1, 0);
        glVertex3f(-1,0.75,0);
        glVertex3f(1,0.75,0);
        glVertex3f(1,1,0);
        glVertex3f(-1, 1, 0);
    glEnd();
}

void nave_2(){
    glColor3f(0.7, 0.2,0.1);
    glBegin(GL_TRIANGLE_FAN);
        glVertex3f(0, 6, 0);
        glVertex3f(1, 4, 0);
        glVertex3f(-1, 4, 0);
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
        glVertex3f(-1,4,0);
        glVertex3f(-1,2,0);
        glVertex3f(1, 2, 0);
        glVertex3f(1, 4, 0);
    glEnd();
        
    glBegin(GL_TRIANGLE_FAN);
        glVertex3f(-1,4,0);
        glVertex3f(-3,0,0);
        glVertex3f(-1, 2, 0);
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
        glVertex3f(1,4,0);
        glVertex3f(3,0,0);
        glVertex3f(1, 2, 0);
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
        glVertex3f(-1, 1, 0);
        glVertex3f(-1,0.75,0);
        glVertex3f(1,0.75,0);
        glVertex3f(1,1,0);
        glVertex3f(-1, 1, 0);
    glEnd();
}

void aviao_aux1(){
    //Largura 6
    //Altura 9.5
    //bico
    glColor3f(0.2,0, 0.4);
    glBegin(GL_TRIANGLE_FAN);
        glVertex3f(0,10,0);
        glVertex3f(-1.5,5,0);
        glVertex3f(1.5,5,0);
    glEnd();

    //corpo
    glBegin(GL_TRIANGLE_FAN);
        glVertex3f(-1.5,5,0);
        glVertex3f(1.5,2,0);
        glVertex3f(-1.5,-7.5,0);
        glVertex3f(1.5,-7.5,0);
        glVertex3f(1.5,5,0);
    glEnd();

    //asa direita
    glBegin(GL_TRIANGLE_FAN);
        glVertex3f(1.5,2,0);
        glVertex3f(1.5,-2.5,0);
        glVertex3f(6,-2.5,0);
    glEnd();

    //asa esquerda
    glBegin(GL_TRIANGLE_FAN);
        glVertex3f(-1.5,2,0);
        glVertex3f(-1.5,-2.5,0);
        glVertex3f(-6,-2.5,0);
    glEnd();

    //asa baixo direita
    glBegin(GL_TRIANGLE_FAN);
        glVertex3f(1.5,-5,0);
        glVertex3f(3.5,-9,0);
        glVertex3f(0,-7.5,0);
    glEnd();   

    //asa baixo esquerda
    glBegin(GL_TRIANGLE_FAN);
        glVertex3f(-1.5,-5,0);
        glVertex3f(-3.5,-9,0);
        glVertex3f(0,-7.5,0);
    glEnd();  

     
    glColor3f(0.5,0.3, 0);
    glBegin(GL_TRIANGLE_FAN);
        glVertex3f(-1.7,1.5,0);
        glVertex3f(-1.7,-2.1,0);
        glVertex3f(-5.4,-2.1,0);
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
        glVertex3f(1.7,1.5,0);
        glVertex3f(1.7,-2.1,0);
        glVertex3f(5.4,-2.1,0);
    glEnd();

}

void aviao_aux2(){
  //Largura 6
    //Altura 9.5
    //bico
    glColor3f(0.1,0.1, 0.2);
    glBegin(GL_TRIANGLE_FAN);
        glVertex3f(0,10,0);
        glVertex3f(-1.5,5,0);
        glVertex3f(1.5,5,0);
    glEnd();

    //corpo
    glBegin(GL_TRIANGLE_FAN);
        glVertex3f(-1.5,5,0);
        glVertex3f(1.5,2,0);
        glVertex3f(-1.5,-7.5,0);
        glVertex3f(1.5,-7.5,0);
        glVertex3f(1.5,5,0);
    glEnd();

    //asa direita
    glBegin(GL_TRIANGLE_FAN);
        glVertex3f(1.5,2,0);
        glVertex3f(1.5,-2.5,0);
        glVertex3f(6,-2.5,0);
    glEnd();

    //asa esquerda
    glBegin(GL_TRIANGLE_FAN);
        glVertex3f(-1.5,2,0);
        glVertex3f(-1.5,-2.5,0);
        glVertex3f(-6,-2.5,0);
    glEnd();

    //asa baixo direita
    glBegin(GL_TRIANGLE_FAN);
        glVertex3f(1.5,-5,0);
        glVertex3f(3.5,-9,0);
        glVertex3f(0,-7.5,0);
    glEnd();   

    //asa baixo esquerda
    glBegin(GL_TRIANGLE_FAN);
        glVertex3f(-1.5,-5,0);
        glVertex3f(-3.5,-9,0);
        glVertex3f(0,-7.5,0);
    glEnd();  

     
    glColor3f(0,1,1);
    glBegin(GL_TRIANGLE_FAN);
        glVertex3f(-1.7,1.5,0);
        glVertex3f(-1.7,-2.1,0);
        glVertex3f(-5.4,-2.1,0);
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
        glVertex3f(1.7,1.5,0);
        glVertex3f(1.7,-2.1,0);
        glVertex3f(5.4,-2.1,0);
    glEnd();


}

void inicializa(){
    memset(&trataTeclado, 0, sizeof(keyboard_t));
    lista[5].P_orient_x = 50;
    lista[5].P_orient_y = -50;
    glutIgnoreKeyRepeat(1);

   //Gero uma lista 
    figure = glGenLists(1);
    glNewList(figure, GL_COMPILE);
        aviao();
    glEndList();

    glClearColor(0.4, 0.5,0.6,0.9);
}

void exibir(void){
    glClear(GL_COLOR_BUFFER_BIT);

    //Figura principal
    glPushMatrix();
        glTranslatef(lista[0].P_orient_x, lista[0].P_orient_y, 0);
        glRotated(lista[0].angulo - 90,0,0,1);
        glCallList(figure);
    glPopMatrix();

    //Aviao_aux 1
    glPushMatrix();
        verifica_crit(cons, 1);
        glTranslatef(lista[1].P_orient_x,lista[1].P_orient_y, 0);
        glRotated(180,0,0,1);
        intersec1(lista[0].P_orient_x, lista[0].P_orient_y, largura_m, altura_m, lista[1].P_orient_x, lista[1].P_orient_y, largura_aviao, altura_aviao);
        aviao_aux1();
    glPopMatrix(); 

    //Aviao_aux2
    glPushMatrix();
        verifica_crit(cons, 2);
        glTranslatef(-40,lista[2].P_orient_y, 0);
        glRotated(180,0,0,1);
        intersec1(lista[0].P_orient_x, lista[0].P_orient_y, largura_m, altura_m, lista[2].P_orient_x, lista[2].P_orient_y, largura_aviao, altura_aviao);
        aviao_aux2();
    glPopMatrix();    
    
    //Nave_1
    glPushMatrix();
        verifica_crit(cons, 3);
        glTranslatef(-60,lista[3].P_orient_y, 0);
        intersec1(lista[0].P_orient_x, lista[0].P_orient_y, largura_m, altura_m, lista[3].P_orient_x, lista[3].P_orient_y, largura_aviao, altura_aviao);
        nave_1();
    glPopMatrix();

    //Nave_2
    glPushMatrix();
        verifica_crit(cons, 4);
        glTranslatef(60,lista[3].P_orient_y, 0);
        intersec1(lista[0].P_orient_x, lista[0].P_orient_y, largura_m, altura_m, lista[4].P_orient_x, lista[4].P_orient_y, largura_aviao, altura_aviao);
        nave_2();
    glPopMatrix();

    glPushMatrix();

    // Missíl
    glPushMatrix();
        glTranslatef(lista[5].P_orient_x,lista[5].P_orient_y, 0);
        intersec1(lista[0].P_orient_x, lista[0].P_orient_y, largura_m, altura_m, lista[5].P_orient_x, lista[5].P_orient_y, 3, 4);
        missil_segue_player();
    glPopMatrix();

    // Missil 2
    glPushMatrix();
        glTranslatef(lista[5].P_orient_x + 20,lista[5].P_orient_y + 10, 0);
        intersec1(lista[0].P_orient_x, lista[0].P_orient_y, largura_m, altura_m, lista[5].P_orient_x + 20, lista[5].P_orient_y + 10, 3, 4);
        missil_segue_player();
    glPopMatrix();
        
        
    glutSwapBuffers();
}

void redimensiona(GLint width, GLint height){   

   //Configuro o aspecto 
   aspectRat = (GLfloat) width / (GLfloat) height;

   glViewport(0, 0, width, height);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(-100 * aspectRat, 100 * aspectRat, -100, 100, -1, 1);
   
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

void teclado(GLubyte tecla, GLint x, GLint y){
    switch (tecla){
        case 27: 
            exit(0); 
            break;

        case('W'):
        case('w'): 
            if(trataTeclado.wPressed == 1){
                trataTeclado.wPressed = 0;
            }
            else{
                trataTeclado.wPressed = 1;
            }
            break;
        case('S'):
        case('s'):
            if(trataTeclado.sPressed == 1){
                trataTeclado.sPressed = 0;
            }else{
                trataTeclado.sPressed = 1;
            }
            break;

        case('D'):
        case('d'):
            if(trataTeclado.dPressed == 1){
                trataTeclado.dPressed = 0;
            }else{
                trataTeclado.dPressed = 1;
            }
            break;

        case('A'):
        case('a'): 
            if(trataTeclado.aPressed == 1){
                trataTeclado.aPressed = 0;
            }else{
                trataTeclado.aPressed = 1;
            }
            break;  
    }
}

void teclado_keys(GLint tecla, GLint x, GLint y){
    switch (tecla){
        case 27: 
            exit(0); 
            break;
        case (GLUT_KEY_UP):
            if(trataTeclado.key_up == 1){
                trataTeclado.key_up = 0;
            }else{
                trataTeclado.key_up = 1;
            }
            break;

        case (GLUT_KEY_DOWN): 
            if(trataTeclado.key_down == 1){
                trataTeclado.key_down = 0;
            }else{
                trataTeclado.key_down = 1;
            }
            break;

        case (GLUT_KEY_RIGHT): 
            if(trataTeclado.key_right == 1){
                trataTeclado.key_right = 0;
            }else{
                trataTeclado.key_right = 1;
            }
            break;
        
        case (GLUT_KEY_LEFT):
            if(trataTeclado.key_left == 1){
                trataTeclado.key_left = 0;
            }else{
                trataTeclado.key_left = 1;
            }
            break;   
    }
}

void updateOnTime(int aux){
    // Movimenta as entidades
    movementPlayer();
    lista[5].P_orient_x += (lista[0].P_orient_x - lista[5].P_orient_x)/50.0f;
    lista[5].P_orient_y += (lista[0].P_orient_y - lista[5].P_orient_y)/50.0f;
    glutIgnoreKeyRepeat(1);

    estaTela();

    glutPostRedisplay();
    glutTimerFunc(aux,updateOnTime,aux);
}

int main(int argc, char **argv){

    // Configuração inicial da janela do GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(100, 100);

    // Abre a janela
    glutCreateWindow("Animation plane");

    inicializa();
    criaListaStruct();

    // Registra as callbacks para tratamento de eventos
    glutDisplayFunc(exibir);
    glutReshapeFunc(redimensiona);
    glutKeyboardFunc(teclado);
    glutKeyboardUpFunc(teclado);
    glutSpecialFunc(teclado_keys);
    glutSpecialUpFunc(teclado_keys);
    glutTimerFunc(17,updateOnTime,17);

    
    glutMainLoop();
    return 0;
}