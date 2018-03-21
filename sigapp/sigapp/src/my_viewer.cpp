
# include "my_viewer.h"

# include <sigogl/ui_button.h>
# include <sigogl/ui_radio_button.h>
# include <sig/sn_primitive.h>
# include <sig/sn_transform.h>
# include <sig/sn_manipulator.h>

# include <sigogl/ws_run.h>
float PI = 3.14f;
static bool _smooth = true;
static GsModel * torus;
static int numFaces = 10;
static float r = 0.1f;
static float R = 0.5f;
GsVec torusFunction(int phi, int theta, const float r1, const float R1) {
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
	float alpha = GS_TORAD(float(phi));
	float beta = GS_TORAD(float(theta));

	x = float(R1 + r1 * cosf(alpha))*cosf(beta);
	y = float(R1 + r1 * cosf(alpha)) * sinf(beta);
	z = float(r1 * sinf(alpha));

	return GsVec(x, y, z);
}
GsVec smoothCalculation(int phi, int theta) {

	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
	float alpha = GS_TORAD(float(phi));
	float beta = GS_TORAD(float(theta));

	x = cosf(alpha) * cosf(beta);
	y = cosf(alpha) * sinf(beta);
	z = sinf(alpha);

	return GsVec(x, y, z);
}

GsVec calculateSurfaceNormal(GsVec u, GsVec v) {
	float nx = 0.0f;
	float ny = 0.0f;
	float nz = 0.0f;

	nx = u.y * v.z - u.z*v.y;
	ny = u.z * v.x - u.x * v.z;
	nz = u.x * v.y - u.y * v.x;

	return GsVec(nx, ny, nz);
}

MyViewer::MyViewer(int x, int y, int w, int h, const char* l) : WsViewer(x, y, w, h, l)
{

	_nbut = 0;
	_animating = false;
	build_ui();
	build_scene();
}

void MyViewer::build_ui()
{
	UiPanel *p, *sp;
	UiManager* uim = WsWindow::uim();
	p = uim->add_panel("", UiPanel::HorizLeft);
	p->add(new UiButton("View", sp = new UiPanel()));
	{	UiPanel* p = sp;
	p->add(_nbut = new UiCheckButton("Normals", EvNormals));
	}
	p->add(new UiButton("Animate", EvAnimate));
	p->add(new UiButton("Exit", EvExit)); p->top()->separate();
}

void MyViewer::add_model(SnShape* s, GsVec p)
{
	SnManipulator* manip = new SnManipulator;
	GsMat m;
	m.translation(p);
	manip->initial_mat(m);

	SnGroup* g = new SnGroup;
	SnLines* l = new SnLines;
	l->color(GsColor::orange);
	g->add(s);
	g->add(l);
	manip->child(g);

	rootg()->add(manip);
}

void MyViewer::build_scene()
{
	SnModel * sn = new SnModel;
	GsModel &m = *sn->model();
	

	int prevPhi = 0;
	int prevTheta = 0;

	int nextPhi = 0;
	int nextTheta = 0;

	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;



	int a = 0;
	int b = 1;
	int c = 2;

	for (nextPhi = numFaces; prevPhi <= 360; nextPhi += numFaces) {
		for (nextTheta = numFaces; nextTheta <= 360; nextTheta += numFaces) {



			GsVec A00 = torusFunction(prevPhi, prevTheta, r, R);
			GsVec A10 = torusFunction(nextPhi, prevTheta, r, R);
			GsVec A01 = torusFunction(prevPhi, nextTheta, r, R);
			GsVec A11 = torusFunction(nextPhi, nextTheta, r, R);

			//0, 1, 2 index
			GsModel::Face f1 = GsModel::Face(a, b, c);
			//3, 4, 5 index 
			GsModel::Face f2 = GsModel::Face(a + 3, b + 3, c + 3);

			/*float u00 = (PI + atan2f(A00.y, A00.x)) / (2.0f * PI);
			float v00 = (1.0f + A00.z) / 2.0f;

			float u10 = (PI + atan2f(A10.y, A10.x)) / (2.0f * PI);
			float v10 = (1.0f + A10.z) / 2.0f;
			
			float u01 = (PI + atan2f(A01.y, A01.x)) / (2.0f * PI);
			float v01 = (1.0f + A01.z) / 2.0f;

			float u11 = (PI + atan2f(A11.y, A11.x)) / (2.0f * PI);
			float v11 = (1.0f + A11.z) / 2.0f;*/

			float v1 = GS_TORAD(float(prevPhi)) / (2.0f * PI); 
			float u1 = GS_TORAD(float(prevTheta)) / (2.0f * PI);
			float v2 = GS_TORAD(float(nextPhi))/ (2.0f * PI);
			float u2 = GS_TORAD(float(nextTheta)) / (2.0f * PI);

			//Push the points that make one triangle
			m.V.push() = A00;
			m.V.push() = A10;
			m.V.push() = A01;
			
			/*m.T.push() = GsPnt2(u00, v00);
			m.T.push() = GsPnt2(u10, v10);
			m.T.push() = GsPnt2(u01, v01);*/
			m.T.push() = GsPnt2(u1, v1);
			m.T.push() = GsPnt2(u2, v1);
			m.T.push() = GsPnt2(u1, v2);
			
			//Push the points that make the other triangle
			m.V.push() = A10;
			m.V.push() = A11;
			m.V.push() = A01;


			m.T.push() = GsPnt2(u1, v2);
			m.T.push() = GsPnt2(u2, v2);
			m.T.push() = GsPnt2(u2, v1);
			//m.T.push() = GsPnt2(u10, v10);
			//m.T.push() = GsPnt2(u11, v11);
			//m.T.push() = GsPnt2(u01, v01);

			//Push the faces of the triangles
			m.F.push() = f1;
			m.F.push() = f2;

			
			
			if (_smooth) {

				//Calculating normals
				A00 = smoothCalculation(prevPhi, prevTheta);
				A10 = smoothCalculation(nextPhi, prevTheta);
				A01 = smoothCalculation(prevPhi, nextTheta);
				A11 = smoothCalculation(nextPhi, nextTheta);

				//Pushing the normals as the triangles
				m.N.push() = A00;
				m.N.push() = A10;
				m.N.push() = A01;

				m.N.push() = A10;
				m.N.push() = A11;
				m.N.push() = A01;

				//torus->set_mode(GsModel::Smooth, GsModel::NoMtl);
			}
			else {
				//Calculating vector u and v. Calculations are provided at https://www.khronos.org/opengl/wiki/Calculating_a_Surface_Normal
				GsVec u = m.V[f1.b] - m.V[f1.a];
				GsVec v = m.V[f1.c] - m.V[f1.a];

				m.N.push() = calculateSurfaceNormal(u, v);

				u = m.V[f2.b] - m.V[f2.a];
				v = m.V[f2.c] - m.V[f2.a];

				m.N.push() = calculateSurfaceNormal(u, v);

				m.set_mode(GsModel::Flat, GsModel::NoMtl);
			}
			a += 6;
			b += 6;
			c += 6;
			prevTheta = nextTheta;

			
		}

		prevPhi = nextPhi;
	}


	GsModel::Group &g = *m.G.push();
	g.fi = 0; 
	g.fn = m.F.size();
	g.dmap = new GsModel::Texture;
	g.dmap->fname.set("..\\textures\\water.jpg");
	
	m.M.push().init();
	//int nv = m.V.size();
	/*m.T.size(nv);
	for (int i = 0; i < nv; ++i) {
		GsVec w = m.V[i];
		float v = acosf(w.y / R) / (2.0f * PI);
		float u = acosf((w.x/ (R + r * cosf(2.0f * PI * v)))) * 2.0f * PI;
		m.T[i] = GsVec2(u, v);
	}*/
	//Texturing is enabled
	m.set_mode(GsModel::Smooth, GsModel::PerGroupMtl);
	m.textured = true;

	rootg()->add(sn);
}

void MyViewer::compute_segments(bool smooth) {

	SnLines * l = new SnLines;
	l->init();
	l->color(GsColor::red);

	if (smooth) {
		GsModel &m = *torus;
		for (int i = 0; i < m.V.size(); ++i) {
			const GsVec& a = m.V[i];

			l->push(a, a + m.N[i] * 0.1f);
		}
	}
	else {
		GsModel &m = *torus;

		for (int i = 0; i < m.F.size(); ++i) {
			const GsVec& a = m.V[m.F[i].a];
			const GsVec& b = m.V[m.F[i].b];
			const GsVec& c = m.V[m.F[i].c];
			GsVec fcenter = (a + b + c) / 3.0f;
			l->push(fcenter, fcenter + m.N[i] * 10.0f);
		}
	}

	rootg()->add(l);
}
// Below is an example of how to control the main loop of an animation:
void MyViewer::run_animation()
{
	if (_animating) return; // avoid recursive calls
	_animating = true;

	int ind = gs_random(0, rootg()->size() - 1); // pick one child
	SnManipulator* manip = rootg()->get<SnManipulator>(ind); // access one of the manipulators
	GsMat m = manip->mat();

	double frdt = 1.0 / 30.0; // delta time to reach given number of frames per second
	double v = 4; // target velocity is 1 unit per second
	double t = 0, lt = 0, t0 = gs_time();
	do // run for a while:
	{
		while (t - lt<frdt) { ws_check(); t = gs_time() - t0; } // wait until it is time for next frame
		double yinc = (t - lt)*v;
		if (t>2) yinc = -yinc; // after 2 secs: go down
		lt = t;
		m.e24 += (float)yinc;
		if (m.e24<0) m.e24 = 0; // make sure it does not go below 0
		manip->initial_mat(m);
		render(); // notify it needs redraw
		ws_check(); // redraw now
	} while (m.e24>0);
	_animating = false;
}

void MyViewer::show_normals(bool b)
{
	// Note that primitives are only converted to meshes in GsModel
	// at the first draw call.
	GsArray<GsVec> fn;
	SnGroup* r = (SnGroup*)root();
	for (int k = 0; k<r->size(); k++)
	{
		SnManipulator* manip = r->get<SnManipulator>(k);
		SnShape* s = manip->child<SnGroup>()->get<SnShape>(0);
		SnLines* l = manip->child<SnGroup>()->get<SnLines>(1);
		if (!b) { l->visible(false); continue; }
		l->visible(true);
		if (!l->empty()) continue; // build only once
		l->init();
		if (s->instance_name() == SnPrimitive::class_name)
		{
			GsModel& m = *((SnModel*)s)->model();
			m.get_normals_per_face(fn);
			const GsVec* n = fn.pt();
			float f = 0.33f;
			for (int i = 0; i<m.F.size(); i++)
			{
				const GsVec& a = m.V[m.F[i].a]; l->push(a, a + (*n++)*f);
				const GsVec& b = m.V[m.F[i].b]; l->push(b, b + (*n++)*f);
				const GsVec& c = m.V[m.F[i].c]; l->push(c, c + (*n++)*f);
			}
		}
	}
}

int MyViewer::handle_keyboard(const GsEvent &e)
{
	int ret = WsViewer::handle_keyboard(e); // 1st let system check events
	if (ret) return ret;

	switch (e.key)
	{
	case 'q': {
		++numFaces;
		rootg()->remove_all();
		build_scene();
		render();
		return 1;
	}
	case 'a': {
		--numFaces;
		rootg()->remove_all();
		build_scene();
		render();
		return 1;
	}
	case 'w': {
		r += 0.1f;
		rootg()->remove_all();
		build_scene();
		render();
		return 1;
	}
	case 's': {
		r -= 0.1f;
		rootg()->remove_all();
		build_scene();
		render();
		return 1;
	}
	case 'e': {
		R += 0.1f;
		rootg()->remove_all();
		build_scene();
		render();
		return 1;
	}
	case 'd': {
		R -= 0.1f;
		rootg()->remove_all();
		build_scene();
		render();
		return 1;
	}
	case 'z': {
		_smooth = false;
		rootg()->remove_all();
		build_scene();
		render();
		return 1;
	}
	case 'x': {
		_smooth = true;
		rootg()->remove_all();
		build_scene();
		render();
		return 1;
	}
	case 'c': {

		compute_segments(_smooth);
		build_scene();
		render();
		return 1;
	}
	case 'v': {

		rootg()->remove_all();
		build_scene();
		render();
		return 1;
	}
	case GsEvent::KeyEsc: gs_exit(); return 1;
	case 'n': { bool b = !_nbut->value(); _nbut->value(b); show_normals(b); return 1; }
	default: gsout << "Key pressed: " << e.key << gsnl;
	}

	return 0;
}

int MyViewer::uievent(int e)
{
	switch (e)
	{
	case EvNormals: show_normals(_nbut->value()); return 1;
	case EvAnimate: run_animation(); return 1;
	case EvExit: gs_exit();
	}
	return WsViewer::uievent(e);
}
