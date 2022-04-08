#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <GL/glut.h>	
static unsigned int YourStep = 0;
static unsigned int opened = 0;
static int gameFinished = 0;

#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/common.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Window.hpp"
#include "GameObj3D.hpp"
#include "ShaderProgram.hpp"
#include "Camera.hpp"
#include "Parametric3DShape.hpp"
#include "CubeData.hpp"
#include "Textures.hpp"
#include "collusion-helpers.hpp"
#include "Scene.hpp"
#include "Mesh.h"
#include "Model.h"
#include "Skybox.hpp"
#include <ft2build.h>
#include FT_FREETYPE_H  

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <random>
#include <iomanip>

static unsigned int Level = 0;

using namespace std;

struct Character {
    unsigned int TextureID; // ID handle of the glyph texture
    glm::ivec2   Size;      // Size of glyph
    glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
    unsigned int Advance;   // Horizontal offset to advance to next glyph
};

std::map<GLchar, Character> Characters;
vector<string> userVec;
vector<int> timeVec;
string OurName;

unsigned int VAO, VBO;
void RenderText(ShaderProgram & shader, std::string text, float x, float y, float scale, glm::vec3 color);

// Globals
int u_transform, u_pv, u_frame, u_light_pos, u_light_color;
int moveFront = 0, moveRight = 0;
float mouseX = 0, mouseY = 0;
float velocityY = 0;
int jump_started = 0;
int jump_finished = 0;
int counter = 0;
int jump_counter = 0;
int current_ball_1 = 1;
int current_ball_2 = 1;
int current_ball_3 = 1;
int cheatmode = 0;


vector<string> OurNamelist = { "Liam", "Noah", "Oliver","Elijah","William","James","Benjamin","Lucas","Henry","Alexander", "Olivia", "Emma", "Ava","Charlotte","Amelia","Isabella","Mia","Evelyn","Harper" };

float float_rand(float min, float max)
{
    float scale = rand() / (float)RAND_MAX; /* [0, 1.0] */
    return min + scale * (max - min);      /* [min, max] */
}

void ParticleMove(int particleID, float minX, float maxX, float minY, float maxY, float minZ, float maxZ)
{
    GameObj3D* object = scene[particleID]->object;
    GameObj* object_g = new GameObj(object, 0);
    object->moveUp(0.0000009f);
    if (object->position().y < maxY - 0.2f && object->position().y >minY + 0.2f && (object->position().x > minX + 0.15f) && (object->position().x < maxX))
    {
        object->moveRight(-0.00005f);
        object->scale(0.018, 0.018, 0.018);
        int randNum = rand() % (13 - 11 + 1) + 11;
        object->textureId = randNum;
    }
    else if (object->position().y < maxY - 0.2f && object->position().y > minY + 0.2f && (object->position().x < minX + 0.15f) && (object->position().x > minX - 0.1f))
    {
        object->moveRight(0.00005f);
        object->scale(0.018, 0.018, 0.018);
        int randNum = rand() % (13 - 11 + 1) + 11;
        object->textureId = randNum;

    }
    else if (object->position().y < minY + 0.2f && (object->position().x < minX + 0.25f) && (object->position().x > maxX))
    {

        object->moveRight(0.00005f);
        object->textureId = 11;
    }
    else if (object->position().y < minY + 0.2f && (object->position().x > minX + 0.25f) && (object->position().x < minX))
    {
        object->moveRight(-0.0005f);
        object->textureId = 11;
    }
    else if (object->position().y > maxY - 0.2f && (object->position().x < minX + 0.25f) && (object->position().x > minX))
    {
        object->moveRight(-0.00005f);
        object->scale(0.013, 0.013, 0.013);
        object->textureId = 13;
    }
    else if (object->position().y > maxY - 0.2f && (object->position().x > minX + 0.25f) && (object->position().x < maxX))
    {
        object->moveRight(0.00005f);
        object->scale(0.013, 0.013, 0.013);
        object->textureId = 13;
    }

    else
    {
        float random = float_rand(minX, maxX);
        float random2 = float_rand(minY, maxY);
        float random3 = float_rand(minZ, maxZ);
        object->scale(0.02, 0.02, 0.02);
        object->translate(random, random2, random3);
        object->textureId = 11;
    }

}


void Velocity()
{
        jump_started = 1;
}


void keyCallback(GLFWwindow *_, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_W && action == GLFW_PRESS)
    {
        moveFront = 1;
    }
    else if (key == GLFW_KEY_W && action == GLFW_RELEASE)
    {
        moveFront = 0;
    }
    if (key == GLFW_KEY_S && action == GLFW_PRESS)
    {
        moveFront = -1;
    }
    else if (key == GLFW_KEY_S && action == GLFW_RELEASE)
    {
        moveFront = 0;
    }
    if (key == GLFW_KEY_D && action == GLFW_PRESS)
    {
        moveRight = 1;
    }
    else if (key == GLFW_KEY_D && action == GLFW_RELEASE)
    {
        moveRight = 0;
    }
    if (key == GLFW_KEY_A && action == GLFW_PRESS)
    {
        moveRight = -1;
    }
    else if (key == GLFW_KEY_A && action == GLFW_RELEASE)
    {
        moveRight = 0;
    }
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
    {
        if (velocityY == 0) 
        {
            Velocity();
        }
       
    }
    if (key == GLFW_KEY_K && action == GLFW_PRESS)
    {
        cheatmode = 1;
        Camera::position = glm::vec3(0, 0, 0);

    }
    else if (key == GLFW_KEY_K && action == GLFW_RELEASE) 
    {

    }
    if (key == GLFW_KEY_L && action == GLFW_PRESS)
    {
        cheatmode = 0;

    }
    else if (key == GLFW_KEY_L && action == GLFW_RELEASE)
    {

    }


}
float fov = 45.0f;
static void cursorPositionCallback(GLFWwindow *_, double x, double y)
{
    mouseX = 2.0 * ((float)x / Window::width) - 1;
    mouseY = 2.0 * (1 - ((float)y / Window::height)) - 1;
}

static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (fov >= 1.0f && fov <= 45.0f)
        fov -= yoffset;
    if (fov <= 1.0f)
        fov = 1.0f;
    if (fov >= 45.0f)
        fov = 45.0f;
}

struct record {
    string word;
    int number;
    int step;
};
vector <record> leaderrecord;

bool compareByLength(const record& a, const record& b)
{
    if (a.step != b.step) 
    {
        return a.step > b.step;
    }
    return a.number < b.number;
}


float calculateDistance(GameObj3D * hero, GameObj3D* newObject)
{
    float x = newObject->position().x - hero->position().x;
    float y = newObject->position().y - hero->position().y;
    float z = newObject->position().z - hero->position().z;

    x = x * x;
    y = y * y;
    z = z * z;

    return x + y + z;
}
int main()
{
    // init window
    Window::init(1200, 1200, "my window");
    glfwSetTime(0);
    ifstream input;
    ofstream output; 

    input.open("leaderboard.txt");

    if (!input.is_open())
    {
        cout << endl;
        cout << "The " << "leaderboard" << " file does not exists";

    }

    vector <string> namelist;

    glfwSetKeyCallback(Window::window, keyCallback);
    glfwSetCursorPosCallback(Window::window, cursorPositionCallback);
    glfwSetScrollCallback(Window::window, scroll_callback);

    // init objects
    Model3D sphereModel = Parametric3DShape::generate(ParametricLine::halfCircle, 4, 4);
    Model3D sphereModel2 = Parametric3DShape::generate(ParametricLine::halfCircle,5,5);
    Model3D sphereModel3 = Parametric3DShape::generate(ParametricLine::halfCircle, 6,6);
    Model3D sphereModel4 = Parametric3DShape::generate(ParametricLine::halfCircle, 10, 10);
    Model3D sphereModel5 = Parametric3DShape::generate(ParametricLine::halfCircle,50, 50);


    Model3D cubeModel(CubeData::positions, CubeData::normals, CubeData::uvs(1,1), CubeData::indices);
    Model3D cubeModel1(CubeData::positions, CubeData::normals, CubeData::uvs_floor(75, 75), CubeData::indices);
   // Model3D cubeModel2(CubeData::positions, CubeData::normals, CubeData::uvs2, CubeData::indices); //front
   // Model3D cubeModel7(CubeData::positions, CubeData::normals, CubeData::uvs7, CubeData::indices);

    Model3D cubeModelonePage(CubeData::positions, CubeData::normals, CubeData::uvs_dollar(1, 1), CubeData::indices);



    GameObj3D hero(cubeModel);
    hero.translate(1, 5.0, 203.0);
    hero.scale(0.3, 0.3, 0.3);
    hero.textureId = 1;
    hero.name = "Hero";

    GameObj* Hero = new GameObj(&hero, 0.0f);
    scene.push_back(Hero);

    
    GameObj3D* Loby_obj = new GameObj3D(cubeModel);
    
    Loby_obj->translate(0, -0.5, 200);
    Loby_obj->scale(100, 0.1, 200);
    Loby_obj->textureId = 1;
    Loby_obj->hasGravity = false;
    Loby_obj->name = "floor";
    GameObj* Loby = new GameObj(Loby_obj, calculateDistance(&hero, Loby_obj));
    scene.push_back(Loby);
   
    //Model ourModel("objects/backpack/backpack.obj");
 
        for(int j = 1; j < 5; j++) 
        {
            GameObj3D* Parcur1 = new GameObj3D(cubeModelonePage);
            
            Parcur1->translate(0 + j, 0.5 * j, 195 - j * 2);
            Parcur1->scale(0.5, 0.5, 0.5);
            Parcur1->textureId = 4;
            Parcur1->hasGravity = false;
            GameObj* Parcur1_g = new GameObj(Parcur1, calculateDistance(&hero, Parcur1));
            scene.push_back(Parcur1_g);
        }

        for (int j = 1; j < 5; j++)
        {
            GameObj3D* Parcur2 = new GameObj3D(cubeModelonePage);
            GameObj* Parcur2_g = new GameObj(Parcur2, calculateDistance(&hero, Parcur2));
            scene.push_back(Parcur2_g);
            Parcur2->translate(5 - j, 2.0 + 0.5 * j, 187 - j * 2);
            Parcur2->scale(0.5, 0.5, 0.5);
            Parcur2->textureId = 4;
            Parcur2->hasGravity = false;  
        }
        
        GameObj3D* Floor1 = new GameObj3D(cubeModelonePage);
        GameObj* Floor1_g = new GameObj(Floor1, calculateDistance(&hero, Floor1));
        scene.push_back(Floor1_g);
        Floor1->translate(1, 4, 176);
        Floor1->scale(5, 0.01, 2);
        Floor1->textureId = 5;
        Floor1->hasGravity = false;
        Floor1->name = "Step1";


        


       for (unsigned int k = 0; k < 5; k++)
        {
            if (k % 2 == 0) 
            {
                GameObj3D* Black = new GameObj3D(cubeModelonePage);
                GameObj* Black_g = new GameObj(Black, calculateDistance(&hero, Black));
                scene.push_back(Black_g);
                Black->translate(-3, 4, 173 - k * 2);
                Black->scale(0.5, 0.5, 0.5);
                Black->WhichOne = k;
                Black->textureId = 6;
                Black->hasGravity = false;
                Black->direction = 1;
            }
            else if(k % 2 != 0)
            {
                GameObj3D* ReverseBlack = new GameObj3D(cubeModelonePage);
                GameObj* ReverseBlack_g = new GameObj(ReverseBlack, calculateDistance(&hero, ReverseBlack));
                scene.push_back(ReverseBlack_g);
                ReverseBlack->translate(3, 4, 173 - k * 2);
                ReverseBlack->scale(0.5, 0.5, 0.5);
                ReverseBlack->WhichOne = k;
                ReverseBlack->textureId = 6;
                ReverseBlack->hasGravity = false;
                ReverseBlack->direction = -1;
            }
        }


        GameObj3D* Floor2 = new GameObj3D(cubeModelonePage);
        GameObj* Floor2_g = new GameObj(Floor2, calculateDistance(&hero, Floor2));
        scene.push_back(Floor2_g);
        Floor2->translate(1, 4, 162);
        Floor2->scale(5, 0.01, 2);
        Floor2->textureId = 5;
        Floor2->hasGravity = false;
        Floor2->name = "Step2";


 

        GameObj3D* Floor3 = new GameObj3D(cubeModelonePage);
        GameObj* Floor3_g = new GameObj(Floor3, calculateDistance(&hero, Floor3));
        scene.push_back(Floor3_g);
        Floor3->translate(1, 4, 150);
        Floor3->scale(2, 0.01, 8);
        Floor3->textureId = 4;
        Floor3->hasGravity = false;

        GameObj3D* Floor4 = new GameObj3D(cubeModelonePage);
        GameObj* Floor4_g = new GameObj(Floor4, calculateDistance(&hero, Floor4));
        scene.push_back(Floor4_g);
        Floor4->translate(1, 4, 136.5f);
        Floor4->scale(5, 0.01, 2);
        Floor4->textureId = 5;
        Floor4->hasGravity = false;
        Floor4->name = "Step3";
        Floor4->direction = 1;
        Floor4->floor_move = 1;

        
        for (int round = 0; round < 4; round++) 
        {
            if (round % 2 == 0) 
            {
                for (int l = 2; l < 6; l++)
                {
                    GameObj3D* Top1 = new GameObj3D(sphereModel);
                    GameObj* Top1_g = new GameObj(Top1, calculateDistance(&hero, Top1));
                    Top1->translate(-2.5 + l, 4.2, 158 - round * 3);
                    Top1->scale(0.2, 0.2, 0.2);
                    Top1->rotate(0, 90, 0);
                    Top1->textureId = 8;
                    Top1->name = "Top1";
                    scene.push_back(Top1_g);

                    //*Top1 = Models[1];

                    GameObj3D* Top2 = new GameObj3D(sphereModel2);
                    GameObj* Top2_g = new GameObj(Top2, calculateDistance(&hero, Top2));
                    Top2->translate(-2.5 + l, 4.2, 158 - round * 3);
                    Top2->scale(0.2, 0.2, 0.2);
                    Top2->rotate(0, 90, 0);
                    Top2->textureId = 8;
                    Top2->name = "Top2";
                    scene.push_back(Top2_g);


                    GameObj3D* Top3 = new GameObj3D(sphereModel3);
                    GameObj* Top3_g = new GameObj(Top3, calculateDistance(&hero, Top3));
                    Top3->translate(-2.5 + l, 4.2, 158 - round * 3);
                    Top3->scale(0.2, 0.2, 0.2);
                    Top3->rotate(0, 90, 0);
                    Top3->textureId = 8;
                    Top3->name = "Top3";
                    scene.push_back(Top3_g);


                    GameObj3D* Top4 = new GameObj3D(sphereModel4);
                    GameObj* Top4_g = new GameObj(Top4, calculateDistance(&hero, Top4));
                    Top4->translate(-2.5 + l, 4.2, 158 - round * 3);
                    Top4->scale(0.2, 0.2, 0.2);
                    Top4->rotate(0, 90, 0);
                    Top4->textureId = 8;
                    Top4->name = "Top4";
                    scene.push_back(Top4_g);

                    GameObj3D* Top5 = new GameObj3D(sphereModel5);
                    GameObj* Top5_g = new GameObj(Top5, calculateDistance(&hero, Top5));
                    Top5->translate(-2.5 + l, 4.2, 158 - round * 3);
                    Top5->scale(0.2, 0.2, 0.2);
                    Top5->rotate(0, 90, 0);
                    Top5->textureId = 8;
                    Top5->name = "Top5";
                    scene.push_back(Top5_g);
                }

                for (int l = 2; l < 6; l++)
                {
                    GameObj3D* Top1 = new GameObj3D(sphereModel);
                    GameObj* Top1_g = new GameObj(Top1, calculateDistance(&hero, Top1));
                    Top1->translate(-2.5 + l, 5.4, 156.5 - round * 3);
                    Top1->scale(0.2, 0.2, 0.2);
                    Top1->rotate(0, 90, 0);
                    Top1->textureId = 8;
                    Top1->name = "Top1";
                    scene.push_back(Top1_g);

                    //*Top1 = Models[1];

                    GameObj3D* Top2 = new GameObj3D(sphereModel2);
                    GameObj* Top2_g = new GameObj(Top2, calculateDistance(&hero, Top2));
                    Top2->translate(-2.5 + l, 5.4, 156.5 - round * 3);
                    Top2->scale(0.2, 0.2, 0.2);
                    Top2->rotate(0, 90, 0);
                    Top2->textureId = 8;
                    Top2->name = "Top2";
                    scene.push_back(Top2_g);


                    GameObj3D* Top3 = new GameObj3D(sphereModel3);
                    GameObj* Top3_g = new GameObj(Top3, calculateDistance(&hero, Top3));
                    Top3->translate(-2.5 + l, 5.4, 156.5 - round * 3);
                    Top3->scale(0.2, 0.2, 0.2);
                    Top3->rotate(0, 90, 0);
                    Top3->textureId = 8;
                    Top3->name = "Top3";
                    scene.push_back(Top3_g);

                    GameObj3D* Top4 = new GameObj3D(sphereModel4);
                    GameObj* Top4_g = new GameObj(Top4, calculateDistance(&hero, Top4));
                    Top4->translate(-2.5 + l, 5.4, 156.5 - round * 3);
                    Top4->scale(0.2, 0.2, 0.2);
                    Top4->rotate(0, 90, 0);
                    Top4->textureId = 8;
                    Top4->name = "Top4";
                    scene.push_back(Top4_g);


                    GameObj3D* Top5 = new GameObj3D(sphereModel5);
                    GameObj* Top5_g = new GameObj(Top5, calculateDistance(&hero, Top5));
                    Top5->translate(-2.5 + l, 5.4, 156.5 - round * 3);
                    Top5->scale(0.2, 0.2, 0.2);
                    Top5->rotate(0, 90, 0);
                    Top5->textureId = 8;
                    Top5->name = "Top5";
                    scene.push_back(Top5_g);
                }
            }
            else if (round % 2 == 1) 
            {
                for (int l = 2; l < 6; l++)
                {
                    GameObj3D* Top1 = new GameObj3D(sphereModel);
                    GameObj* Top1_g = new GameObj(Top1, calculateDistance(&hero, Top1));
                    Top1->translate(-2 + l, 4.2, 158 - round * 3);
                    Top1->scale(0.2, 0.2, 0.2);
                    Top1->rotate(0, 90, 0);
                    Top1->textureId = 8;
                    Top1->name = "Top1";
                    scene.push_back(Top1_g);

                    //*Top1 = Models[1];

                    GameObj3D* Top2 = new GameObj3D(sphereModel2);
                    GameObj* Top2_g = new GameObj(Top2, calculateDistance(&hero, Top2));
                    Top2->translate(-2 + l, 4.2, 158 - round * 3);
                    Top2->scale(0.2, 0.2, 0.2);
                    Top2->rotate(0, 90, 0);
                    Top2->textureId = 8;
                    Top2->name = "Top2";
                    scene.push_back(Top2_g);


                    GameObj3D* Top3 = new GameObj3D(sphereModel3);
                    GameObj* Top3_g = new GameObj(Top3, calculateDistance(&hero, Top3));
                    Top3->translate(-2 + l, 4.2, 158 - round * 3);
                    Top3->scale(0.2, 0.2, 0.2);
                    Top3->rotate(0, 90, 0);
                    Top3->textureId = 8;
                    Top3->name = "Top3";
                    scene.push_back(Top3_g);


                    GameObj3D* Top4 = new GameObj3D(sphereModel4);
                    GameObj* Top4_g = new GameObj(Top4, calculateDistance(&hero, Top4));
                    Top4->translate(-2 + l, 4.2, 158 - round * 3);
                    Top4->scale(0.2, 0.2, 0.2);
                    Top4->rotate(0, 90, 0);
                    Top4->textureId = 8;
                    Top4->name = "Top4";
                    scene.push_back(Top4_g);


                    GameObj3D* Top5 = new GameObj3D(sphereModel5);
                    GameObj* Top5_g = new GameObj(Top5, calculateDistance(&hero, Top5));
                    Top5->translate(-2 + l, 4.2, 158 - round * 3);
                    Top5->scale(0.2, 0.2, 0.2);
                    Top5->rotate(0, 90, 0);
                    Top5->textureId = 8;
                    Top5->name = "Top5";
                    scene.push_back(Top5_g);
                }

                for (int l = 2; l < 6; l++)
                {
                    GameObj3D* Top1 = new GameObj3D(sphereModel);
                    GameObj* Top1_g = new GameObj(Top1, calculateDistance(&hero, Top1));
                    Top1->translate(-2 + l, 5.4, 156.5 - round * 3);
                    Top1->scale(0.2, 0.2, 0.2);
                    Top1->rotate(0, 90, 0);
                    Top1->textureId = 8;
                    Top1->name = "Top1";
                    scene.push_back(Top1_g);

                    //*Top1 = Models[1];

                    GameObj3D* Top2 = new GameObj3D(sphereModel2);
                    GameObj* Top2_g = new GameObj(Top2, calculateDistance(&hero, Top2));
                    Top2->translate(-2 + l, 5.4, 156.5 - round * 3);
                    Top2->scale(0.2, 0.2, 0.2);
                    Top2->rotate(0, 90, 0);
                    Top2->textureId = 8;
                    Top2->name = "Top2";
                    scene.push_back(Top2_g);


                    GameObj3D* Top3 = new GameObj3D(sphereModel3);
                    GameObj* Top3_g = new GameObj(Top3, calculateDistance(&hero, Top3));
                    Top3->translate(-2 + l, 5.4, 156.5 - round * 3);
                    Top3->scale(0.2, 0.2, 0.2);
                    Top3->rotate(0, 90, 0);
                    Top3->textureId = 8;
                    Top3->name = "Top3";
                    scene.push_back(Top3_g);


                    GameObj3D* Top4 = new GameObj3D(sphereModel4);
                    GameObj* Top4_g = new GameObj(Top4, calculateDistance(&hero, Top4));
                    Top4->translate(-2 + l, 5.4, 156.5 - round * 3);
                    Top4->scale(0.2, 0.2, 0.2);
                    Top4->rotate(0, 90, 0);
                    Top4->textureId = 8;
                    Top4->name = "Top4";
                    scene.push_back(Top4_g);

                    GameObj3D* Top5 = new GameObj3D(sphereModel5);
                    GameObj* Top5_g = new GameObj(Top5, calculateDistance(&hero, Top5));
                    Top5->translate(-2 + l, 5.4, 156.5 - round * 3);
                    Top5->scale(0.2, 0.2, 0.2);
                    Top5->rotate(0, 90, 0);
                    Top5->textureId = 8;
                    Top5->name = "Top5";
                    scene.push_back(Top5_g);
                }
            }
           
           
        } 


        for (int black = 0; black < 2; black++)
        {
            for (int s = 0; s < 4; s++)
            {
                GameObj3D* Brown = new GameObj3D(cubeModelonePage);
                GameObj* Brown_g = new GameObj(Brown, calculateDistance(&hero, Brown));
                scene.push_back(Brown_g);
                Brown->translate(1 + s, 4 + s * 0.5f + black * 7, 132 - s);
                Brown->scale(0.5, 0.3, 0.5);
                Brown->WhichOne = 1;
                Brown->textureId = 10;
                Brown->hasGravity = false;
                Brown->direction = 1;
            }

            for (int s2 = 0; s2 < 3; s2++)
            {
                GameObj3D* Brown2 = new GameObj3D(cubeModelonePage);
                GameObj* Brown2_g = new GameObj(Brown2, calculateDistance(&hero, Brown2));
                scene.push_back(Brown2_g);
                Brown2->translate(3 - s2, 6 + s2 * 0.5f + black * 7, 128 - s2);
                Brown2->scale(0.5, 0.3, 0.5);
                Brown2->WhichOne = 2;
                Brown2->textureId = 10;
                Brown2->hasGravity = false;
                Brown2->direction = 1;
            }


            for (int s3 = 0; s3 < 4; s3++)
            {
                GameObj3D* Brown3 = new GameObj3D(cubeModelonePage);
                GameObj* Brown3_g = new GameObj(Brown3, calculateDistance(&hero, Brown3));
                scene.push_back(Brown3_g);
                Brown3->translate(0 - s3, 7.5f + s3 * 0.5f + black * 7, 127 + s3);
                Brown3->scale(0.5, 0.3, 0.5);
                Brown3->WhichOne = 3;
                Brown3->textureId = 10;
                Brown3->hasGravity = false;
                Brown3->direction = 1;
            }

            for (int s4 = 0; s4 < 3; s4++)
            {
                GameObj3D* Brown4 = new GameObj3D(cubeModelonePage);
                GameObj* Brown4_g = new GameObj(Brown4, calculateDistance(&hero, Brown4));
                scene.push_back(Brown4_g);
                Brown4->translate(-2 + s4, 9.5f + s4 * 0.5f + black * 7, 131 + s4);
                Brown4->scale(0.5, 0.3, 0.5);
                Brown4->WhichOne = 4;
                Brown4->textureId = 10;
                Brown4->hasGravity = false;
                Brown4->direction = 1;
            }

            for (int s5 = 0; s5 < 5; s5++)
            {
                GameObj3D* Brown4 = new GameObj3D(cubeModelonePage);
                GameObj* Brown4_g = new GameObj(Brown4, calculateDistance(&hero, Brown4));
                scene.push_back(Brown4_g);
                Brown4->translate(0, 17.5f, 133 - s5 * 2);
                Brown4->scale(0.5, 0.3, 0.5);
                Brown4->WhichOne = 4;
                Brown4->textureId = 10;
                Brown4->hasGravity = false;
                Brown4->direction = 1;
            }



        }
          

        GameObj3D* Floor5 = new GameObj3D(cubeModelonePage);
        GameObj* Floor5_g = new GameObj(Floor5, calculateDistance(&hero, Floor5));
        scene.push_back(Floor5_g);
        Floor5->translate(0, 17.5, 113);
        Floor5->scale(5, 0.01, 10);
        Floor5->textureId = 2;
        Floor5->hasGravity = false;
        Floor5->name = "Step4";



        GameObj3D* Dollar = new GameObj3D(cubeModelonePage);
        Dollar->translate(0, 18.5f, 102);
        Dollar->scale(3, 1, 0.5);
        Dollar->textureId = 3;
        Dollar->hasGravity = false;
        GameObj* Dollar_g = new GameObj(Dollar, calculateDistance(&hero, Dollar));
        scene.push_back(Dollar_g);

        
        int dir = 1;
        for (int j = 1; j < 3; j++)
        {
            GameObj3D* sphere_enemy = new GameObj3D(sphereModel5);
            GameObj* sphere_enemy_g = new GameObj(sphere_enemy, calculateDistance(&hero, sphere_enemy));
            scene.push_back(sphere_enemy_g);
            sphere_enemy->translate(dir * 4, 19, 118 - (j * 2));
            sphere_enemy->scale(0.5, 0.5, 0.5);
            sphere_enemy->textureId = 7;
            sphere_enemy->hasGravity = false;
            sphere_enemy->direction = dir * 1;
            dir = dir * -1;

        }
        for (int j = 1; j < 3; j++)
        {
            GameObj3D* sphere_enemy = new GameObj3D(sphereModel5);
            GameObj* sphere_enemy_g = new GameObj(sphere_enemy, calculateDistance(&hero, sphere_enemy));
            scene.push_back(sphere_enemy_g);
            sphere_enemy->translate(dir * 4, 19, 112 - (j * 2));
            sphere_enemy->scale(0.5, 0.5, 0.5);
            sphere_enemy->textureId = 7;
            sphere_enemy->hasGravity = false;
            sphere_enemy->direction = dir * -1;
            dir = dir * -1;
        }

        //PARTICLE DENEMESI
        for (int j = 1; j < 250; j++)
        {
            float random = (3 + 1) + (((float)rand()) / (float)RAND_MAX) * (3.5 - (3 + 1));
            float random2 = (9 + 1) + (((float)rand()) / (float)RAND_MAX) * (10.5 - (9 + 1));
            float random3 = (149 + 1) + (((float)rand()) / (float)RAND_MAX) * (149.5 - (149 + 1));
            GameObj3D* particle = new GameObj3D(sphereModel3);
            GameObj* particle_g = new GameObj(particle, 0);
            scene.push_back(particle_g);
            particle->translate(random, random2, random3);
            particle->scale(0.02, 0.02, 0.02);
            particle->textureId = 11;
            particle->name = "First_particle";
        }

        for (int j = 1; j < 250; j++)
        {
            float random = (3 + 1) + (((float)rand()) / (float)RAND_MAX) * (3.5 - (3 + 1));
            float random2 = (9 + 1) + (((float)rand()) / (float)RAND_MAX) * (10.5 - (9 + 1));
            float random3 = (149 + 1) + (((float)rand()) / (float)RAND_MAX) * (149.5 - (149 + 1));
            GameObj3D* particle = new GameObj3D(sphereModel3);
            GameObj* particle_g = new GameObj(particle, 0);
            scene.push_back(particle_g);
            particle->translate(random, random2, random3);
            particle->scale(0.02, 0.02, 0.02);
            particle->textureId = 11;
            particle->name = "Second_particle";
        }
       
   

    // light
    glm::vec3 lightPos = glm::vec3(0.0, 0.0, 1.0);
    glm::vec3 lightColor = glm::vec3(1.0, 1.0, 1.0);

    const vector<string> texture_files{ "gold.jpg", "grass2.jpg", "bottom.jpg", "Dolar.jpg", "Parkur.jpg","Sabancý.jpg", "Parkur2.jpg", "wall.jpg", "disc.jpg", "dead.jpg", "step4.jpg", "bottom_fire.jpg", "top_fire.jpg","fire_yellow.jpg" };
    // load textures
    vector<unsigned int> textures = Textures::loadTextures(texture_files);


    // load skybox
    unsigned int skyboxVAO, skyboxVBO;
    initSkybox(skyboxVAO, skyboxVBO);
    vector<std::string> faces{
        "right.jpg",
        "left.jpg",
        "top.jpg",
        "bottom.jpg",
        "front.jpg",
        "back.jpg" };
    unsigned int cubemapTexture = loadCubemap(faces);
    ShaderProgram skyboxShader("skybox.vert", "skybox.frag");
    skyboxShader.use();
    auto skybox_texture = glGetUniformLocation(skyboxShader.id, "skybox");
    auto u_pv_sky = glGetUniformLocation(skyboxShader.id, "u_pv");
    glUniform1i(skybox_texture, 0); // 0th unit



   



   
    
    ShaderProgram textsp("textvertex.vert", "textfrag.frag");
    textsp.use();
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(1200), 0.0f, static_cast<float>(1200));
   
    glUniformMatrix4fv(glGetUniformLocation(textsp.id, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    FT_Library ft;
    // All functions return a value different than 0 whenever an error occurred
    if (FT_Init_FreeType(&ft))
    {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return -1;
    }

    // find path to font
    std::string font_name = "Antonio-Bold.ttf";
    if (font_name.empty())
    {
        std::cout << "ERROR::FREETYPE: Failed to load font_name" << std::endl;
        return -1;
    }
    FT_Face face;
    if (FT_New_Face(ft, font_name.c_str(), 0, &face)) {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
        return -1;
    }
    else {
        // set size to load glyphs as
        FT_Set_Pixel_Sizes(face, 0, 48);

        // disable byte-alignment restriction
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        // load first 128 characters of ASCII set
        for (unsigned char c = 0; c < 128; c++)
        {
            // Load character glyph 
            if (FT_Load_Char(face, c, FT_LOAD_RENDER))
            {
                std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
                continue;
            }
            // generate texture
            unsigned int texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
            );
            // set texture options
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            // now store character for later use
            Character character = {
                texture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                static_cast<unsigned int>(face->glyph->advance.x)
            };
            Characters.insert(std::pair<char, Character>(c, character));
        }
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    

    // create shader
    ShaderProgram sp("vertex.vert", "frag.frag");
    sp.use();
    u_transform = glGetUniformLocation(sp.id, "u_transform");
    u_pv = glGetUniformLocation(sp.id, "u_pv");
    u_frame = glGetUniformLocation(sp.id, "u_frame");
    u_light_pos = glGetUniformLocation(sp.id, "u_light_pos");
    u_light_color = glGetUniformLocation(sp.id, "u_light_color");
    auto u_texture = glGetUniformLocation(sp.id, "u_texture");
    glUniform1i(u_texture, 0);
    glActiveTexture(GL_TEXTURE0); // active 0th unit

    unsigned a = 0;
    // game loop
    while (!Window::isClosed())
    {
        // update player and camera
        if (cheatmode == 0)
        {
            hero.moveFront(moveFront * 0.2);
            hero.rotate(hero.rotation().x, hero.rotation().y - moveRight * 3.0f, hero.rotation().z);
        }

        if (jump_counter < 10 && jump_started)
        {
            jump_counter += 1;
        }

        if(jump_counter != 0 && jump_counter != 10) {
            hero.moveUp(0.4f);
        }

        if (jump_counter == 10 && jump_finished == 0) {
            jump_finished = 10;
        }

        if (jump_finished != 0) 
        {
            jump_finished -= 1;
        }
        // for jump logic
     
        if ((jump_finished == 0 && jump_counter == 10))
        {
            jump_started = 0;
            jump_counter = 0;
            velocityY = 0;
        }

        if (cheatmode == 0) 
        {
            Camera::position = hero.position() - hero.front() * 4.0f + hero.up() * 2.0f;

            Camera::front = hero.front() + glm::vec3(0, mouseY, 0);
            Camera::up = glm::cross(Camera::front, hero.right());
            Camera::front = glm::rotateY(Camera::front, -mouseX);
        }
        else if (cheatmode == 1)
        {
            Camera::rotation = glm::vec3(180 * mouseY, -180 * mouseX, 0);
            Camera::moveFront(moveFront * 0.1);
            Camera::moveRight(moveRight * 0.1);
        }

        // update uniforms
        sp.use();
        if (cheatmode == 0) 
        {
            glUniformMatrix4fv(u_pv, 1, GL_FALSE, glm::value_ptr(Camera::getProjMatrix()* Camera::getViewMatrix()));
        }
        else if (cheatmode == 1)
        {
            glUniformMatrix4fv(u_pv, 1, GL_FALSE, glm::value_ptr(Camera::getProjMatrix()* Camera::get2ViewMatrix()));
        }
       
        glUniform1i(u_frame, 1);
        glUniform3fv(u_light_pos, 1, glm::value_ptr(lightPos));
        glUniform3fv(u_light_color, 1, glm::value_ptr(lightColor));


        //glEnable(GL_FRAMEBUFFER_SRGB);

        // scene draw
        for (std::vector<GameObj * >::iterator t = scene.begin(); t != scene.end(); ++t) {

            // get the object
            const int i = t - scene.begin();


            // gravity logic
            GameObj3D* object = scene[i]->object;
            GameObj3D* orj_hero = scene[0]->object;

         
            if (object->hasGravity) {
                object->moveUp(-0.2f);             
            }

            if (object->textureId == 6)
            {
                int k = object->WhichOne;
                
                object->moveRight(0.05f * object->direction);

                if (object->position().x > 5.0f || object->position().x < -5.0f)
                   {  
                        object->direction = object->direction * -1;
                   }  
                
            }

            if (object->textureId == 5 && object->floor_move == 1) 
            {
                object->moveFront(0.04f * object->direction);

                if (object->position().z < 135.0f || object->position().z > 139.5f)
                {
                    object->direction = object->direction * -1;
                }
            }
           
            if ((object->textureId == 11 || object->textureId == 12 || object->textureId == 13)) //particle yukarÄ± hareket etmeli
            {
                float minX = -5.0f;
                float maxX = 5.0f;
                float minY = 17.6f;
                float maxY = 18.2f;
                float minZ = 115.0f;
                float maxZ = 115.5f;
                if (object->name == "First_particle")
                {
                    ParticleMove(object->id, minX, maxX, minY, maxY, minZ, maxZ);
                }
                else if (object->name == "Second_particle")
                {
                    ParticleMove(object->id, minX , maxX , minY, maxY, minZ-6, maxZ-6);
                }
            }
            if (object->textureId == 7)
            {
                int k = object->WhichOne;

                object->moveRight(0.07f * object->direction);

                if (object->position().x > 4.5f || object->position().x < -4.5f)
                {
                    object->direction = object->direction * -1;
                }

            }


            
      
            sp.use();
            // draw the object

            glUniformMatrix4fv(u_transform, 1, GL_FALSE, glm::value_ptr(object->getTransform()));
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, textures[object->textureId]);

            if (object->name != "Top1" && object->name != "Top2" && object->name != "Top3" && object->name != "Top4" && object->name != "Top5")
            {
                object->draw();
            }
            else if(scene[i]->distance <= 16.0f && object->name == "Top5")
            {
                object->draw();
            }
            else if (scene[i]->distance <= 36.0f && object->name == "Top4" && scene[i]->distance > 16.0f)
            {
                object->draw();
            }
            else if (scene[i]->distance <= 64.0f && object->name == "Top3" && scene[i]->distance > 36.0f)
            {
                object->draw();
            }
            else if (scene[i]->distance <= 81.0f && object->name == "Top2" && scene[i]->distance > 64.0f)
            {
                object->draw();
            }
            else if (object->name == "Top1" && scene[i]->distance > 81.0f)
            {
                object->draw();
            }
            
            if (object->name != "particle") 
            {
                scene[i]->distance = calculateDistance(&hero, scene[i]->object);
            }
          
           
        }

        // draw skybox
        glDepthFunc(GL_LEQUAL); // change depth function so depth test passes when values are equal to depth buffer's content
        skyboxShader.use();
        glUniformMatrix4fv(u_pv_sky, 1, GL_FALSE, glm::value_ptr(Camera::getProjMatrix() * glm::mat4(glm::mat3(Camera::getViewMatrix()))));
        // skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        textsp.use();


        if (opened == 0)
        {
            string records;

            while (getline(input, records)) {
                string user;
                int time;
                int step;
                stringstream ss(records);
                ss >> user >> time >> step;

                record OurRecord = { user,time, step };
                userVec.push_back(user);
                timeVec.push_back(time);
                namelist.push_back(user);

                leaderrecord.push_back(OurRecord);
            }
            input.close();
            opened = 1;

            if (leaderrecord.size() != 0)
            {
                std::sort(leaderrecord.begin(), leaderrecord.end(), compareByLength);
            }

          
            bool namefound = false;
            bool nameinside = false;
            while (!namefound)
            {
                int randNum = rand() % ((OurNamelist.size() - 1) - 0 + 1) + 0;
                OurName = OurNamelist[randNum];
                nameinside = false;


                for (int keyNum = 0; keyNum < namelist.size(); keyNum++)
                {
                    if (OurName == namelist[keyNum])
                    {
                        nameinside = true;
                    }
                }

                if (nameinside != true)
                {
                    namefound = true;
                }
            

                if (namelist.size() == OurNamelist.size())
                {
                    cout << "List is full";
                    break;
                }
            }

        }



        RenderText(textsp, OurName, 900.0f, 980.0f, 0.5f, glm::vec3(0.3, 0.7f, 0.9f));

        Level = glfwGetTime();
        if (Level < 60)
        {
            RenderText(textsp, "Time: " + to_string(Level), 1000.0f, 980.0f, 0.5f, glm::vec3(0.3, 0.7f, 0.9f));
        }
        else if (Level < 3600)
        {
            int minute = Level / 60;
            int second = Level % 60;
            string msg = to_string(minute) + ":" + to_string(second);
            RenderText(textsp, msg, 1000.0f, 980.0f, 0.5f, glm::vec3(0.3, 0.7f, 0.9f));
        }

        

        


        if (gameFinished == 1)
        {
            glfwSetWindowShouldClose(Window::window, GLFW_TRUE);
        }

        

        RenderText(textsp, "The LeaderBoard", 30, 1000.0f, 0.5f, glm::vec3(0.3, 0.7f, 0.9f));

        if (leaderrecord.size() != 0)
        {
            if (leaderrecord.size() > 5)
            {
                float iteator = 0.0f;
                for (int i = 0; i < 5; i++)
                {
                    string msg2 = leaderrecord[i].word;
                    RenderText(textsp, msg2, 30, 960.0f - iteator, 0.5f, glm::vec3(0.3, 0.7f, 0.9f));
                    if (leaderrecord[i].number < 60)
                    {
                        RenderText(textsp, to_string(leaderrecord[i].number), 150, 960.0f - iteator, 0.5f, glm::vec3(0.3, 0.7f, 0.9f));
                    }
                    else if (leaderrecord[i].number < 3600)
                    {
                        int minute = leaderrecord[i].number / 60;
                        int second = leaderrecord[i].number % 60;
                        string msg = to_string(minute) + ":" + to_string(second);
                        RenderText(textsp, msg, 150, 960.0f - iteator, 0.5f, glm::vec3(0.3, 0.7f, 0.9f));
                    }
                    string msg3 = "Level: " + to_string(leaderrecord[i].step);
                    RenderText(textsp, msg3, 200, 960.0f - iteator, 0.5f, glm::vec3(0.3, 0.7f, 0.9f));
                    iteator += 60;
                }
            }
            else 
            {
                float iteator = 0.0f;
                for (int i = 0; i < leaderrecord.size(); i++)
                {
                    string msg2 = leaderrecord[i].word;
                    RenderText(textsp, msg2, 30, 960.0f - iteator, 0.5f, glm::vec3(0.3, 0.7f, 0.9f));
                    if (leaderrecord[i].number < 60)
                    {
                        RenderText(textsp, to_string(leaderrecord[i].number), 150, 960.0f - iteator, 0.5f, glm::vec3(0.3, 0.7f, 0.9f));
                    }
                    else if (leaderrecord[i].number < 3600)
                    {
                        int minute = leaderrecord[i].number / 60;
                        int second = leaderrecord[i].number % 60;
                        string msg = to_string(minute) + ":" + to_string(second);
                        RenderText(textsp, msg, 150, 960.0f - iteator, 0.5f, glm::vec3(0.3, 0.7f, 0.9f));
                    }
                    string msg3 = "Level: " + to_string(leaderrecord[i].step);
                    RenderText(textsp, msg3, 200, 960.0f - iteator, 0.5f, glm::vec3(0.3, 0.7f, 0.9f));
                    iteator += 60;
                }
            }  
        }
        else
        {
            string msg2 = "Empty";
            RenderText(textsp, msg2, 30, 960.0f, 0.5f, glm::vec3(0.3, 0.7f, 0.9f));
        }
        

        RenderText(textsp, "Press W for move forward", 900.0f, 950.0f, 0.5f, glm::vec3(0.3, 0.7f, 0.9f));
        RenderText(textsp, "Press S for move backward", 900.0f, 920.0f, 0.5f, glm::vec3(0.3, 0.7f, 0.9f));
        RenderText(textsp, "Press A for turn left", 900.0f, 890.0f, 0.5f, glm::vec3(0.3, 0.7f, 0.9f));
        RenderText(textsp, "Press D for turn right", 900.0f, 860.0f, 0.5f, glm::vec3(0.3, 0.7f, 0.9f));
        RenderText(textsp, "Press SPACE for jump", 900.0f, 830.0f, 0.5f, glm::vec3(0.3, 0.7f, 0.9f));

        RenderText(textsp, "Your Level: " + to_string(YourStep), 900.0f, 800.0f, 0.5f, glm::vec3(0.3, 0.7f, 0.9f));


        // update the scene
        Window::refresh();
    }


    output.open("leaderboard.txt", ios::in | ios::out | ios::ate);
    Level = glfwGetTime();
    output << OurName << " " << to_string(Level) << " " << to_string(YourStep) << endl;
    output.close();

    Window::terminate();


    


    return 0;
}

void RenderText(ShaderProgram & shader, std::string text, float x, float y, float scale, glm::vec3 color)
{
    // activate corresponding render state	
    shader.use();
    glUniform3f(glGetUniformLocation(shader.id, "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    // iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = Characters[*c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}