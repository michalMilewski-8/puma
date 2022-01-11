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
    void DrawFrame(float T, glm::vec3 start_pos, glm::vec3 end_pos, glm::vec3 rotation_start, glm::vec3 rotation_end, bool aproximation_is_line = true);
    void DrawFrame(float T, glm::vec3 start_pos, glm::vec3 end_pos, glm::quat rotation_start, glm::quat rotation_end, bool aproximation_is_line = true);

    float l1 = 2.0f;
    float l3 = 2.0f;
    float l4 = 2.0f;

private:
    void create_block_points();
    void update_object() override;

    std::pair<std::vector<float>, std::vector<unsigned int>> generate_cylinder(float r, float l,bool start_in_center, glm::vec3 dir, glm::vec3 col);

    glm::vec3 view_pos;
    bool blocks_need_creation = true;

    unsigned int texture;
    unsigned int VBOb[10];
    unsigned int VAOb[10];
    unsigned int EBOb[10];


    float q2 = 4.0f;
    float a1 = 30, a2 = 30, a3 = 10, a4 = 15, a5 = 30;

    std::unique_ptr<Cursor> cur;

    std::vector<float> points[10];
    std::vector<unsigned int> quads[10];
};


