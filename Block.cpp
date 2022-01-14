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

void Block::DrawFrame(float T)
{
	float tmp;
	tmp = abs(start.a1 - end.a1);;
	if (tmp > M_PI)
		tmp = -(2 * M_PI - tmp);
	if (end.a1 > start.a1)
		current.a1 = start.a1 + T * tmp;
	else
		current.a1 = start.a1 - T * tmp;

	tmp = abs(start.a2 - end.a2);;
	if (tmp > M_PI)
		tmp = -(2 * M_PI - tmp);
	if (end.a2 > start.a2)
		current.a2 = start.a2 + T * tmp;
	else
		current.a2 = start.a2 - T * tmp;

	tmp = abs(start.a3 - end.a3);;
	if (tmp > M_PI)
		tmp = -(2 * M_PI - tmp);
	if (end.a3 > start.a3)
		current.a3 = start.a3 + T * tmp;
	else
		current.a3 = start.a3 - T * tmp;

	tmp = abs(start.a4 - end.a4);;
	if (tmp > M_PI)
		tmp = -(2 * M_PI - tmp);
	if (end.a4 > start.a4)
		current.a4 = start.a4 + T * tmp;
	else
		current.a4 = start.a4 - T * tmp;

	tmp = abs(start.a5 - end.a5);;
	if (tmp > M_PI)
		tmp = -(2 * M_PI - tmp);
	if (end.a5 > start.a5)
		current.a5 = start.a5 + T * tmp;
	else
		current.a5 = start.a5 - T * tmp;

	current.q2 = start.q2 + T * (end.q2 - start.q2);


}

void Block::DrawFrame(float T, glm::vec3 start_pos, glm::vec3 end_pos, glm::quat rotation_start, glm::quat rotation_end)
{
	last = current;
	if (T > 1.0f) T = 1.0f;
	if (T < 0.0f) T = 0.0f;
	if (T == 0.0f) {
		current = start;
		return;
	}
	auto current_pos = start_pos + (end_pos - start_pos) * T;
	auto cur_rot = glm::slerp(rotation_start, rotation_end, T);

	SolveInverse(current_pos, cur_rot);

	current = configuration::choose_closer(first, second, last);
}

void Block::create_block_points()
{
	auto res = generate_cylinder(0.3, 0.3, true, { 0,1,0 }, { 1,0,0 });
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

	res = generate_cylinder(0.1, 1.0f, false, { 1,0,0 }, { 1,1,0 });
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

float Block::angle(glm::vec3 v, glm::vec3 w, glm::vec3 dir)
{
	auto c = glm::normalize(glm::cross(v, w));
	float a;

	a = glm::acos(glm::dot(v, w) / (glm::length(v) * glm::length(w)));

	if (glm::length2(dir - c) > 1e-4)
		return -a;
	else
		return a;
}

std::pair<std::vector<float>, std::vector<unsigned int>> Block::generate_cylinder(float r, float l, bool start_in_center, glm::vec3 dir, glm::vec3 col)
{
	std::vector<float> points_ = {};
	std::vector<unsigned int> triangles_ = {};

	std::vector<glm::vec4> circle = {};

	float delta = 2.0f * M_PI / 200.0f;

	for (int i = 0; i < 200; i++) {
		circle.push_back({ r * glm::sin(delta * i), r * glm::cos(delta * i), glm::sin(delta * i), glm::cos(delta * i) });
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
		triangles_.push_back(1 + i);
		triangles_.push_back(1 + (i + 1) % 200);

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

	return { points_, triangles_ };
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
	glm::mat4 model = translate * rotate * resize;
	model = glm::rotate(model, (float)current.a1, { 0,1,0 });
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
	model = glm::rotate(model, (float)(current.a2), { 0,0,-1 });

	trmodel = glm::transpose(glm::inverse(model));
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(model));

	glUniformMatrix4fv(trmodelLoc, 1, GL_FALSE, glm::value_ptr(trmodel));

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawElements(GL_TRIANGLES, quads[2].size(), GL_UNSIGNED_INT, 0);

	glBindVertexArray(VAOb[3]);

	auto model2 = model;

	model2 = glm::scale(model2, { current.q2,1,1 });

	trmodel = glm::transpose(glm::inverse(model2));
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(model2));

	glUniformMatrix4fv(trmodelLoc, 1, GL_FALSE, glm::value_ptr(trmodel));

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawElements(GL_TRIANGLES, quads[3].size(), GL_UNSIGNED_INT, 0);

	// 3
	glBindVertexArray(VAOb[4]);
	model = glm::translate(model, { current.q2,0,0 });
	model = glm::rotate(model, (float)(current.a3), { 0,0,-1 });

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
	model = glm::rotate(model, (float)(current.a4), { 0,1,0 });

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
	model = glm::rotate(model, (float)((current.a5)), { 1,0,0 });

	trmodel = glm::transpose(glm::inverse(model));
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(model));

	glUniformMatrix4fv(trmodelLoc, 1, GL_FALSE, glm::value_ptr(trmodel));

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawElements(GL_TRIANGLES, quads[8].size(), GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);

	cur->DrawObject(mvp * model);
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

std::tuple<float, float, float, float, float, float> Block::SolveInverse(glm::vec3 pos, glm::quat q) {

	glm::vec3 x5 = glm::rotate(q, { 1,0,0,0 });
	glm::vec3 y5 = glm::rotate(q, { 0,0,-1,0 });
	glm::vec3 z5 = glm::rotate(q, { 0,1,0,0 });
	glm::vec3 x0 = { 1,0,0 };


	glm::vec3 z0 = { 0,1,0 };
	glm::vec3 p0 = { 0,0,0 };
	glm::vec3 p5 = pos;
	glm::vec3 p1 = p0;
	glm::vec3 p2 = p0 + l1 * z0;
	glm::vec3 p4 = p5 - l4 * x5;

	glm::vec3 norm = glm::cross(p4 - p0, z0);

	if (glm::length2(norm) < 1e-4) {
		if((glm::dot(p5 -p0,x5) < 0.0f && glm::dot(p4 - p0, x5) > 0.0f)|| (glm::dot(p5 - p0, x5) > 0.0f && glm::dot(p4 - p0, x5) < 0.0f))
			norm = -glm::normalize(glm::cross(p5 - p0, z0));
		else
			norm = glm::normalize(glm::cross(p5 - p0, z0));
	}
	else {
		norm = glm::normalize(norm);
	}

	glm::vec3 z4 = {};

	if (std::abs(x5.y) < 1e-4) {
		z4 = { 0,1,0 };
	}
	else if (abs(norm.z) < 1e-4) {
		float c1 = -norm.z / norm.x;
		float c2 = -(x5.x * c1 + x5.z) / x5.y;
		z4.z = std::sqrtf(1 / (1 + c1 * c1 + c2 * c2));
		z4.y = z4.z * c2;
		z4.x = z4.z * c1;
	}
	else {
		float c1 = -norm.x / norm.z;
		float c2 = -(x5.z * c1 + x5.x) / x5.y;
		z4.x = std::sqrtf(1 / (1 + c1 * c1 + c2 * c2));
		z4.y = z4.x * c2;
		z4.z = z4.x * c1;
	}

	{
		// pierwsze rozwiazanie
		glm::vec3 p3 = p4 + l3 * z4;

		glm::mat4 rotation = glm::mat4(1.0f);

		float a1_ = std::atan2f(-p4.z, p4.x);

		if (std::abs(p4.z) < 1e-4 && std::abs(p4.x) < 1e-4)
			a1_ = std::atan2f(-p5.z, p5.x);

		rotation = glm::rotate(rotation, a1_, { 0,1,0 });

		glm::vec3 x1 = rotation * glm::vec4(x0, 0);

		float a2_ = angle(p2 - p0, p3 - p2, rotation * glm::vec4{ 0,0,-1,0 }) - M_PI_2;
		if (glm::length2(p3 - p2) < 1e-4)
			a2_ = angle(p2 - p0, x1, rotation * glm::vec4{ 0,0,-1,0 }) - M_PI_2;
		float q2_ = glm::length(p3 - p2);

		rotation = glm::rotate(rotation, a2_, { 0,0,-1 });

		float a3_ = angle(p3 - p2, p4 - p3, rotation * glm::vec4{ 0,0,-1,0 }) - M_PI_2;
		if (glm::length2(p3 - p2) < 1e-4)
			a3_ = angle(x1, p4 - p3, rotation * glm::vec4{ 0,0,-1,0 }) - M_PI_2;

		rotation = glm::rotate(rotation, a3_, { 0,0,-1 });

		float a4_ = angle(-norm, x5, rotation * glm::vec4{ 0,1,0,0 }) + M_PI_2;

		rotation = glm::rotate(rotation, a4_, { 0,1,0 });

		float a5_ = angle(p3 - p4, z5, rotation * glm::vec4{ 1,0,0,0 });

		current.a1 = a1_;
		current.a2 = a2_;
		current.a3 = a3_;
		current.a4 = a4_;
		current.a5 = a5_;
		current.q2 = q2_;

		first = current;
	}
	{
		glm::vec3 p3 = p4 - l3 * z4;

		glm::mat4 rotation = glm::mat4(1.0f);

		float a1_ = std::atan2f(-p4.z, p4.x);

		if (std::abs(p4.z) < 1e-4 && std::abs(p4.x) < 1e-4)
			a1_ = std::atan2f(-p5.z, p5.x);
		rotation = glm::rotate(rotation, a1_, { 0,1,0 });

		glm::vec3 x1 = rotation * glm::vec4(x0, 0);

		float a2_ = angle(p2 - p0, p3 - p2, rotation * glm::vec4{ 0,0,-1,0 }) - M_PI_2;
		if (glm::length2(p3 - p2) < 1e-4)
			a2_ = angle(p2 - p0, x1, rotation * glm::vec4{ 0,0,-1,0 }) - M_PI_2;
		float q2_ = glm::length(p3 - p2);

		rotation = glm::rotate(rotation, a2_, { 0,0,-1 });

		float a3_ = angle(p3 - p2, p4 - p3, rotation * glm::vec4{ 0,0,-1,0 }) - M_PI_2;
		if (glm::length2(p3 - p2) < 1e-4)
			a3_ = angle(x1, p4 - p3, rotation * glm::vec4{ 0,0,-1,0 }) - M_PI_2;

		rotation = glm::rotate(rotation, a3_, { 0,0,-1 });

		float a4_ = angle(-norm, x5, rotation * glm::vec4{ 0,1,0,0 }) + M_PI_2;

		rotation = glm::rotate(rotation, a4_, { 0,1,0 });

		float a5_ = angle(p3 - p4, z5, rotation * glm::vec4{ 1,0,0,0 });

		second.a1 = a1_;
		second.a2 = a2_;
		second.a3 = a3_;
		second.a4 = a4_;
		second.a5 = a5_;
		second.q2 = q2_;
	}

	return {};
}

configuration configuration::choose_closer(configuration c1, configuration c2, configuration l)
{
	float dist1 = 0.0f;
	float dist2 = 0.0f;

	float tmp = 0.0f;

	tmp = abs(l.a1 - c1.a1);
	if (tmp > M_PI)
		tmp = 2 * M_PI - tmp;
	dist1 += tmp * tmp;

	tmp = abs(l.a2 - c1.a2);
	if (tmp > M_PI)
		tmp = 2 * M_PI - tmp;
	dist1 += tmp * tmp;

	tmp = abs(l.a3 - c1.a3);
	if (tmp > M_PI)
		tmp = 2 * M_PI - tmp;
	dist1 += tmp * tmp;

	tmp = abs(l.a4 - c1.a4);
	if (tmp > M_PI)
		tmp = 2 * M_PI - tmp;
	dist1 += tmp * tmp;

	tmp = abs(l.a5 - c1.a5);
	if (tmp > M_PI)
		tmp = 2 * M_PI - tmp;
	dist1 += tmp * tmp;

	tmp = abs(l.q2 - c1.q2);
	dist1 += tmp * tmp;

	tmp = abs(l.a1 - c2.a1);
	if (tmp > M_PI)
		tmp = 2 * M_PI - tmp;
	dist2 += tmp * tmp;

	tmp = abs(l.a2 - c2.a2);
	if (tmp > M_PI)
		tmp = 2 * M_PI - tmp;
	dist2 += tmp * tmp;

	tmp = abs(l.a3 - c2.a3);
	if (tmp > M_PI)
		tmp = 2 * M_PI - tmp;
	dist2 += tmp * tmp;

	tmp = abs(l.a4 - c2.a4);
	if (tmp > M_PI)
		tmp = 2 * M_PI - tmp;
	dist2 += tmp * tmp;

	tmp = abs(l.a5 - c2.a5);
	if (tmp > M_PI)
		tmp = 2 * M_PI - tmp;
	dist2 += tmp * tmp;

	tmp = abs(l.q2 - c2.q2);
	dist2 += tmp * tmp;

	if (abs(dist1 - dist2) < 1e-2) 
		std::cout << "debug";
	if (dist1 <= dist2)
		return c1;
	else
		return c2;
}
