#include "Block.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <fstream>

void Block::SetViewPos(glm::vec3 view_pos)
{
	this->view_pos = view_pos;
}

void Block::Update()
{
	need_update = true;
}

void Block::DrawFrame(float T, glm::vec3 start_pos, glm::vec3 end_pos, glm::vec3 rotation_start, glm::vec3 rotation_end, bool aproximation_is_line)
{
	rotate = glm::mat4(0.0f);

	this->MoveObjectTo(start_pos + (end_pos - start_pos) * T);

	glm::vec3 rotation_diff = rotation_end - rotation_start;

	if (abs(rotation_diff.x) > abs(rotation_end.x - (360.0f + rotation_start.x)))
		rotation_diff.x = rotation_end.x - (360.0f + rotation_start.x);
	if (abs(rotation_diff.y) > abs(rotation_end.y - (360.0f + rotation_start.y)))
		rotation_diff.y = rotation_end.y - (360.0f + rotation_start.y);
	if (abs(rotation_diff.z) > abs(rotation_end.z - (360.0f + rotation_start.z)))
		rotation_diff.z = rotation_end.z - (360.0f + rotation_start.z);

	if (abs(rotation_diff.x) > abs(360.0f + rotation_end.x -  rotation_start.x))
		rotation_diff.x = 360.0f + rotation_end.x - rotation_start.x;
	if (abs(rotation_diff.y) > abs(360.0f + rotation_end.y - rotation_start.y))
		rotation_diff.y = 360.0f + rotation_end.y - rotation_start.y;
	if (abs(rotation_diff.z) > abs(360.0f + rotation_end.z - rotation_start.z))
		rotation_diff.z = 360.0f + rotation_end.z - rotation_start.z;


	this->RotateObject(EulerToQuaternion(rotation_start + rotation_diff * T));

}

void Block::DrawFrame(float T, glm::vec3 start_pos, glm::vec3 end_pos, glm::quat rotation_start, glm::quat rotation_end, bool aproximation_is_line)
{
	rotate = glm::mat4(0.0f);
	this->MoveObjectTo(start_pos + (end_pos - start_pos) * T);
	if (aproximation_is_line) {
		if (glm::length(rotation_end - rotation_start) < glm::length(-rotation_end - rotation_start))
			this->RotateObject(glm::normalize(rotation_start + (rotation_end - rotation_start) * T));
		else
			this->RotateObject(glm::normalize(rotation_start + (-rotation_end - rotation_start) * T));
	}
	else {
		this->RotateObject(glm::slerp(rotation_start, rotation_end, T));
	}
}

void Block::create_block_points()
{
	auto res = generate_cylinder(0.3, 0.3, true, { 0,1,0 }, {1,0,0});
	for (auto& pt : res.first) {
		points[0].push_back(pt);
	}
	for (auto& pt : res.second) {
		quads[0].push_back(pt);
	}

	res = generate_cylinder(0.1, l1, false, { 0,1,0 }, { 1,0,0 });
	for (auto& pt : res.first) {
		points[1].push_back(pt);
	}
	for (auto& pt : res.second) {
		quads[1].push_back(pt);
	}

	res = generate_cylinder(0.3, 0.3, true, { 0,0,-1 }, { 1,1,0 });
	for (auto& pt : res.first) {
		points[2].push_back(pt);
	}
	for (auto& pt : res.second) {
		quads[2].push_back(pt);
	}

	res = generate_cylinder(0.1, q2, false, { 1,0,0 }, { 1,1,0 });
	for (auto& pt : res.first) {
		points[3].push_back(pt);
	}
	for (auto& pt : res.second) {
		quads[3].push_back(pt);
	}

	res = generate_cylinder(0.3, 0.3, true, { 0,0,-1 }, { 0,1,0 });
	for (auto& pt : res.first) {
		points[4].push_back(pt);
	}
	for (auto& pt : res.second) {
		quads[4].push_back(pt);
	}

	res = generate_cylinder(0.1, l3, false, { 0,-1,0 }, { 0,1,0 });
	for (auto& pt : res.first) {
		points[5].push_back(pt);
	}
	for (auto& pt : res.second) {
		quads[5].push_back(pt);
	}

	res = generate_cylinder(0.3, 0.3, true, { 0,1,0 }, { 0,0,1 });
	for (auto& pt : res.first) {
		points[6].push_back(pt);
	}
	for (auto& pt : res.second) {
		quads[6].push_back(pt);
	}

	res = generate_cylinder(0.1, l4, false, { 1,0,0 }, { 0,0,1 });
	for (auto& pt : res.first) {
		points[7].push_back(pt);
	}
	for (auto& pt : res.second) {
		quads[7].push_back(pt);
	}

	res = generate_cylinder(0.3, 0.3, true, { 1,0,0 }, { 1,0,1 });
	for (auto& pt : res.first) {
		points[8].push_back(pt);
	}
	for (auto& pt : res.second) {
		quads[8].push_back(pt);
	}
}

void Block::update_object()
{
	if (blocks_need_creation) {
		create_block_points();

		shader.use();
		for (int i = 0; i < 10; i++) {
			// 1. bind Vertex Array Object
			glBindVertexArray(VAOb[i]);
			// 2. copy our vertices array in a vertex buffer for OpenGL to use
			glBindBuffer(GL_ARRAY_BUFFER, VBOb[i]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * points[i].size(), points[i].data(), GL_DYNAMIC_DRAW);
			// 3. copy our index array in a element buffer for OpenGL to use
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOb[i]);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * quads[i].size(), quads[i].data(), GL_DYNAMIC_DRAW);
			// 4. then set the vertex attributes pointers
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, description_number * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, description_number * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, description_number * sizeof(float), (void*)(6 * sizeof(float)));
			glEnableVertexAttribArray(2);
		}


		blocks_need_creation = false;
	}
}

std::pair<std::vector<float>, std::vector<unsigned int>> Block::generate_cylinder(float r, float l, bool start_in_center,glm::vec3 dir, glm::vec3 col)
{
	std::vector<float> points_ = {};
	std::vector<unsigned int> triangles_ = {};

	std::vector<glm::vec4> circle = {};

	float delta = 2.0f * M_PI / 200.0f;

	for (int i = 0; i < 200; i++) {
		circle.push_back({ r*glm::sin(delta * i), r*glm::cos(delta * i), glm::sin(delta * i), glm::cos(delta * i) });
	}

	glm::vec3 start;
	if (start_in_center) {
		start = -dir * (l / 2.0f);
	}
	else {
		start = { 0,0,0 };
	}
	points_.push_back(start.x);
	points_.push_back(start.y);
	points_.push_back(start.z);

	points_.push_back(-dir.x);
	points_.push_back(-dir.y);
	points_.push_back(-dir.z);

	points_.push_back(col.x);
	points_.push_back(col.y);
	points_.push_back(col.z);

	for (const auto& pt : circle) {
		if (dir.x != 0) {
			points_.push_back(start.x);
			points_.push_back(pt.x);
			points_.push_back(pt.y);

			points_.push_back(0);
			points_.push_back(pt.z);
			points_.push_back(pt.w);
		}
		else if (dir.y != 0) {
			points_.push_back(pt.x);
			points_.push_back(start.y);
			points_.push_back(pt.y);

			points_.push_back(pt.z);
			points_.push_back(0);
			points_.push_back(pt.w);
		}
		else {
			points_.push_back(pt.x);
			points_.push_back(pt.y);
			points_.push_back(start.z);

			points_.push_back(pt.z);
			points_.push_back(pt.w);
			points_.push_back(0);
		}
		points_.push_back(col.x);
		points_.push_back(col.y);
		points_.push_back(col.z);
	}

	start += l * dir;

	points_.push_back(start.x);
	points_.push_back(start.y);
	points_.push_back(start.z);

	points_.push_back(dir.x);
	points_.push_back(dir.y);
	points_.push_back(dir.z);

	points_.push_back(col.x);
	points_.push_back(col.y);
	points_.push_back(col.z);

	for (const auto& pt : circle) {
		if (dir.x != 0) {
			points_.push_back(start.x);
			points_.push_back(pt.x);
			points_.push_back(pt.y);

			points_.push_back(0);
			points_.push_back(pt.z);
			points_.push_back(pt.w);
		}
		else if (dir.y != 0) {
			points_.push_back(pt.x);
			points_.push_back(start.y);
			points_.push_back(pt.y);

			points_.push_back(pt.z);
			points_.push_back(0);
			points_.push_back(pt.w);
		}
		else {
			points_.push_back(pt.x);
			points_.push_back(pt.y);
			points_.push_back(start.z);

			points_.push_back(pt.z);
			points_.push_back(pt.w);
			points_.push_back(0);
		}
		points_.push_back(col.x);
		points_.push_back(col.y);
		points_.push_back(col.z);
	}

	for (int i = 0; i < 200; i++) {
		triangles_.push_back(0);
		triangles_.push_back(1+i);
		triangles_.push_back(1+(i+1)%200);

		triangles_.push_back(201);
		triangles_.push_back(200 + 2 + i);
		triangles_.push_back(200 + 2 + (i + 1) % 200);

		triangles_.push_back(1 + i);
		triangles_.push_back(1 + (i + 1) % 200);
		triangles_.push_back(200 + 2 + i);

		triangles_.push_back(200 + 2 + (i + 1) % 200);
		triangles_.push_back(200 + 2 + i);
		triangles_.push_back(1 + (i + 1) % 200);
	}

	return { points_, triangles_};
}

Block::Block(Shader sh) :
	Object(sh, 9)
{
	cur = std::make_unique<Cursor>(sh);
	glGenBuffers(10, EBOb);
	glGenVertexArrays(10, VAOb);
	glGenBuffers(10, VBOb);
	shader = Shader("shader_tex.vert", "shader_tex.frag");
	for (int i = 0; i < 10; i++) {
		points[i].clear();
		quads[i].clear();
	}
	
	update_object();
}

void Block::DrawObject(glm::mat4 mvp)
{
	if (need_update) {
		update_object();
		need_update = false;
	}

	Object::DrawObject(mvp);

	glm::mat4 vp = mvp;
	shader.use();

	int mvLoc = glGetUniformLocation(shader.ID, "vp");
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(vp));

	int viewPos = glGetUniformLocation(shader.ID, "viewPos");
	glUniform3fv(viewPos, 1, &view_pos[0]);

	// 1
	glBindVertexArray(VAOb[0]);
	glm::mat4 model = translate*rotate*resize;
	model = glm::rotate(model, (float)(a1/180.0f * M_PI), { 0,1,0 });
	glm::mat4 trmodel = glm::transpose(glm::inverse(model));
	int projectionLoc = glGetUniformLocation(shader.ID, "model");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(model));

	int trmodelLoc = glGetUniformLocation(shader.ID, "trmodel");
	glUniformMatrix4fv(trmodelLoc, 1, GL_FALSE, glm::value_ptr(trmodel));

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawElements(GL_TRIANGLES, quads[0].size(), GL_UNSIGNED_INT, 0);

	glBindVertexArray(VAOb[1]);

	model = model;

	trmodel = glm::transpose(glm::inverse(model));
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(model));

	glUniformMatrix4fv(trmodelLoc, 1, GL_FALSE, glm::value_ptr(trmodel));

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawElements(GL_TRIANGLES, quads[1].size(), GL_UNSIGNED_INT, 0);

	// 2
	glBindVertexArray(VAOb[2]);
	model = glm::translate(model, { 0,l1,0 });
	model = glm::rotate(model, (float)(a2 / 180.0f * M_PI), {0,0,-1});

	trmodel = glm::transpose(glm::inverse(model));
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(model));

	glUniformMatrix4fv(trmodelLoc, 1, GL_FALSE, glm::value_ptr(trmodel));

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawElements(GL_TRIANGLES, quads[2].size(), GL_UNSIGNED_INT, 0);

	glBindVertexArray(VAOb[3]);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawElements(GL_TRIANGLES, quads[3].size(), GL_UNSIGNED_INT, 0);

	// 3
	glBindVertexArray(VAOb[4]);
	model = glm::translate(model, { q2,0,0 });
	model = glm::rotate(model, (float)(a3 / 180.0f * M_PI), { 0,0,-1 });

	trmodel = glm::transpose(glm::inverse(model));
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(model));

	glUniformMatrix4fv(trmodelLoc, 1, GL_FALSE, glm::value_ptr(trmodel));

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawElements(GL_TRIANGLES, quads[4].size(), GL_UNSIGNED_INT, 0);

	glBindVertexArray(VAOb[5]);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawElements(GL_TRIANGLES, quads[5].size(), GL_UNSIGNED_INT, 0);

	// 4
	glBindVertexArray(VAOb[6]);
	model = glm::translate(model, { 0,-l3,0 });
	model = glm::rotate(model, (float)((a4) / 180.0f * M_PI), { 0,1,0 });

	trmodel = glm::transpose(glm::inverse(model));
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(model));

	glUniformMatrix4fv(trmodelLoc, 1, GL_FALSE, glm::value_ptr(trmodel));

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawElements(GL_TRIANGLES, quads[6].size(), GL_UNSIGNED_INT, 0);

	glBindVertexArray(VAOb[7]);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawElements(GL_TRIANGLES, quads[7].size(), GL_UNSIGNED_INT, 0);

	// 5
	glBindVertexArray(VAOb[8]);
	model = glm::translate(model, { l4,0,0 });
	model = glm::rotate(model, (float)((a5) / 180.0f * M_PI), { 1,0,0 });

	trmodel = glm::transpose(glm::inverse(model));
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(model));

	glUniformMatrix4fv(trmodelLoc, 1, GL_FALSE, glm::value_ptr(trmodel));

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawElements(GL_TRIANGLES, quads[8].size(), GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);

	cur->DrawObject(mvp* model);
}

glm::quat Block::EulerToQuaternion(glm::vec3 rot)
{
	//glm::vec3 Ox = { 1,0,0 };
	//glm::vec3 Oy = { 0,1,0 };
	//glm::vec3 Oz = { 0,0,1 };

	//rot = rot / (float)(180.0f * M_PI);

	//glm::quat quat = {0,0,0,1};
	//auto q = glm::angleAxis(rot.x, Oz);
	//Ox = q * Ox;
	//Oy = q * Oy;
	//Oz = q * Oz;
	//quat = q * quat;

	//q = glm::angleAxis(rot.y, Ox);
	//Ox = q * Ox;
	//Oy = q * Oy;
	//Oz = q * Oz;
	//quat = q * quat;

	//q = glm::angleAxis(rot.z, Oz);
	//Ox = q * Ox;
	//Oy = q * Oy;
	//Oz = q * Oz;
	//quat = q * quat;

	//return quat;

	rot = (rot / 180.0f) * (float)M_PI;
	return  glm::quat(rot);
}

glm::vec3 Block::QuaternionToEuler(glm::quat quat)
{
	return (glm::eulerAngles(quat) / (float)M_PI) * 180.0f;
}
