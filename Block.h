#pragma once
#include "Object.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <math.h>
#define _USE_MATH_DEFINES
#include <cmath>
#include "Cursor.h"

struct configuration {
    float a1;
    float a2;
    float q2;
    float a3;
    float a4;
    float a5;

    static configuration choose_closer(configuration c1, configuration c2, configuration l);
};

class Block :
    public Object
{
public:
    Block(Shader sh);

    void DrawObject(glm::mat4 mvp) override;

    static glm::quat EulerToQuaternion(glm::vec3 rot);
    static glm::vec3 QuaternionToEuler(glm::quat quat);

    void SetViewPos(glm::vec3 view_pos);
    void Update() override;
    void DrawFrame(float T, glm::vec3 start_pos, glm::vec3 end_pos, glm::quat rotation_start, glm::quat rotation_end);
    void DrawFrame(float T);

    std::tuple<float, float, float, float, float, float> SolveInverse(glm::vec3 pos, glm::quat q);

    float l1 = 2.0f;
    float l3 = 2.0f;
    float l4 = 2.0f;

    configuration current = { 0,0,2,0,0,0 };
    configuration last = { 0,0,2,0,0,0 };

    configuration start = { 0,0,2,0,0,0 };
    configuration end = { 0,0,2,0,0,0 };

    configuration first = { 0,0,2,0,0,0 };
    configuration second = { 0,0,2,0,0,0 };



private:
    void create_block_points();
    void update_object() override;
    float angle(glm::vec3 v, glm::vec3 w, glm::vec3 dir);

    std::pair<std::vector<float>, std::vector<unsigned int>> generate_cylinder(float r, float l,bool start_in_center, glm::vec3 dir, glm::vec3 col);

    glm::vec3 view_pos;
    bool blocks_need_creation = true;

    unsigned int texture;
    unsigned int VBOb[10];
    unsigned int VAOb[10];
    unsigned int EBOb[10];

    std::unique_ptr<Cursor> cur;

    std::vector<float> points[10];
    std::vector<unsigned int> quads[10];
};


