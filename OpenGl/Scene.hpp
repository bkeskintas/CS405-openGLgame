#pragma once
#include "GameObj3D.hpp"


struct GameObj
{
    GameObj3D* object;
    float distance;

    GameObj();
    GameObj(GameObj3D* obj, float mesafe) : object(obj), distance(mesafe) {}
};

vector<GameObj * > scene;
#include "irrKlang-64bit-1.6.0/include/irrKlang.h"
using namespace irrklang;

ISoundEngine* SoundEngine = createIrrKlangDevice();
#pragma comment(lib, "irrKlang.lib") // link with irrKlang.dll




void DeleteFromScene(int id)
{
    int index = -1;
    for (int i = 0; i < scene.size(); i++)
    {
        if (scene[i]->object->id == id)
        {
            index = i;
            break;
        }
    }
    if (index > -1)
    {
        delete scene[index];
        scene.erase(scene.begin() + index);
    }
}


bool CollidesWithSth(GameObj3D &check)
{
    
    for (int i = 0; i < scene.size(); i++)
    {
        GameObj3D *obj = scene[i]->object;

        if (obj->isCollid && check.id != obj->id)
        {
            if (intersect(check, *obj))
            {

                if (( obj->textureId == 2 || obj->textureId == 5) && (obj->achieved == 0) && check.name == "Hero")
                {
                    SoundEngine->play2D("audio/powerup.wav", false);
                    obj->achieved = 1;
                    
                }

                if(obj->textureId == 3 && check.name == "Hero")
                {
                    gameFinished = 1;
                }

                if (obj->name == "Step1")
                {
                    YourStep = 1;
                }
                else if (obj->name == "Step2")
                {
                    YourStep = 2;
                }
                else if (obj->name == "Step3")
                {
                    YourStep = 3;
                }
                else if ((obj->name == "floor" && check.name == "Hero") || (check.name == "Hero" && (obj->name == "First_particle" || obj->name == "Second_particle")))
                {
                    if (YourStep == 1) 
                    {
                        check.translate(1, 5, 176);
                    }
                    else if (YourStep == 2)
                    {
                        check.translate(1, 5, 162);
                    }
                    else if (YourStep == 3) 
                    {
                        GameObj3D* floor = scene[18]->object;
                        check.translate(floor->position().x, floor->position().y + 1, floor->position().z);

                    }
                    else if (YourStep == 4) 
                    {
                        check.translate(0, 18.5, 125);
                    }
                }
                else if (obj->name == "Step4" && check.textureId != 11)
                {
                    YourStep = 4;
                }
                
                if ((obj->textureId == 11 && check.textureId == 11) || (obj->textureId == 12 && check.textureId == 12) || (obj->textureId == 11 && check.textureId == 12) || (obj->textureId == 12 && check.textureId == 11) || (obj->textureId == 13 && check.textureId == 13) || (obj->textureId == 12 && check.textureId == 13) || (obj->textureId == 13 && check.textureId == 12) || (obj->textureId == 13 && check.textureId == 11) || (obj->textureId == 11 && check.textureId == 13))
                {
                    return false;
                }


                return true;
            }
        }
    }
    return false;
}

