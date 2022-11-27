#include "stdio.h"
#include "stdlib.h"
#include "assert.h"

// ------------------- Math utils -------------------

const double eps = 1e-6;
double abs_d(double a) {
	return a < 0.0 ? -a : a;
}
int eq(double a, double b) {
	return abs_d(a - b) < eps;
}
int leq(double a, double b) {
	return b - a > -eps;
}
double sgn(double a) {
	if(eq(a, 0.0)) return 0.0;
	if(leq(a, 0.0)) return -1.0;
	return 1.0;
}
// whether x is inside [a, b]
int inside_interval(double x, double a, double b) {
	return leq(a, x) && leq(x, b);
}

// ------------------- Vector2 -------------------

typedef struct Vector2 {
	double x, y;
} Vector2;

void input_vector(Vector2 *v) {
	assert(scanf("%lf %lf", &v->x, &v->y));
}
// return vector (a - b)
Vector2 subtract_v2(Vector2 a, Vector2 b) {
	return (Vector2){.x = a.x - b.x, .y = a.y - b.y};
}
// whether the two vectors are the same
int eq_v2(Vector2 a, Vector2 b) {
	return eq(a.x, b.x) && eq(a.y, b.y);
}
// distance between points squared
double dist_v2(Vector2 a, Vector2 b) {
	return (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y);
}
// dot product of two vectors
double dot(Vector2 a, Vector2 b) {
	return a.x * b.x + a.y * b.y;
}
// cross product of two vectors
double cross(Vector2 a, Vector2 b) {
	return a.x * b.y - b.x * a.y;
}
// ------------------- Rect_data -------------------

// Rect_data representation: points a, b, where a is the bottom-left corner and b is the top-right corner
typedef struct Rect_data {
	Vector2 a, b;
} Rect_data;

void input_rect(Rect_data *data) {
	input_vector(&data->a);
	input_vector(&data->b);
}
// whether point p is inside rectangle rect
int inside_rect(Vector2 p, Rect_data rect) {
	return inside_interval(p.x, rect.a.x, rect.b.x) && inside_interval(p.y, rect.a.y, rect.b.y);
}

// ------------------- Circle_data -------------------

typedef struct Circle_data {
	Vector2 p;
	double r;
} Circle_data;

void input_circle(Circle_data *data) {
	input_vector(&data->p);
	assert(scanf("%lf", &data->r));
}
// whether point p is inside circle circ
int inside_circle(Vector2 p, Circle_data circ) {
	return leq(dist_v2(p, circ.p), circ.r * circ.r);
}

// ------------------- Line -------------------

// Line representation: A, B, C, such that Ax + By + C = 0; p being an example point on the line and u being the original vector
typedef struct Line {
	double A, B, C;
	Vector2 p, u;
} Line;

void input_line(Line *l) {
	Vector2 p, q;
	input_vector(&p);
	input_vector(&q);
	l->u = subtract_v2(q, p); // v is parallel to the line
	l->A = l->u.y;
	l->B = -l->u.x;
	l->C = -(l->A * p.x + l->B * p.y);
	l->p = p;
}
// return the given point after applying line symmetry
Vector2 line_symmetry(Vector2 p, Line l) {
	double dist = abs_d(l.A * p.x + l.B * p.y + l.C) / (l.A * l.A + l.B * l.B); // distance(P, l) / length([A, B])
	Vector2 n = (Vector2){.x = l.A, .y = l.B};
	double s = sgn(dot(n, subtract_v2(p, l.p))); // 1.0 if the angle between the Line-Point vector and the normal vector
												 // is less than pi, -1.0 if more than pi, 0.0 if exactly pi
	n.x *= s * dist;
	n.y *= s * dist;
	// now n is a vector from l to p, perpendicular to l
	return subtract_v2(subtract_v2(p, n), n); // the resulting point is p - 2n
}

// ------------------- Shape -------------------

// Shape type
typedef enum Type {
	Rect_type,
	Circle_type,
	Folded_type
} Type;

typedef struct Shape {
	Type type; // shape type
	union // shape initialisation data
	{
		Rect_data r; // for Rect
		Circle_data c; // for Circle
	} init_data;
	Line trans; // last transformation
	int prev_trans; // index of the previous transformation
} Shape;

void input_shape(Shape *shape) {
	char c;
	assert(scanf("\n%c", &c));
	switch (c) {
		case 'P':
			shape->type = Rect_type;
			input_rect(&shape->init_data.r);
			break;
		case 'K':
			shape->type = Circle_type;
			input_circle(&shape->init_data.c);
			break;
		case 'Z':
			shape->type = Folded_type;
			assert(scanf("%d", &shape->prev_trans));
			--shape->prev_trans;
			input_line(&shape->trans);
			break;
	}
}

// ------------------- QUERIES -------------------

// if the given shape is folded, then unfold the specified point for the next shape,
// otherwise check if the point is inside the shape and update the counter
void unfold_point(int k, Vector2 p, int *counter, Shape* shapes) {
	Vector2 p2;
	switch (shapes[k].type) {
		case Folded_type:
			if(sgn(cross(shapes[k].trans.u, subtract_v2(p, shapes[k].trans.p))) < 0.0) return; // the point is to the right of the line
			p2 = line_symmetry(p, shapes[k].trans);
			unfold_point(shapes[k].prev_trans, p, counter, shapes);
			if(!eq_v2(p, p2)) unfold_point(shapes[k].prev_trans, p2, counter ,shapes); // a different point was produced after the symmetry
			break;
		case Rect_type:
			if(inside_rect(p, shapes[k].init_data.r)) ++(*counter);
			break;
		case Circle_type:
			if(inside_circle(p, shapes[k].init_data.c)) ++(*counter);
			break;
	}
}

// query calculation - returns the number of layers at the p point on the k-th shape
int calculate_query(int k, Vector2 p, Shape *shapes) {
	int strikes_counter = 0;
	unfold_point(k, p, &strikes_counter, shapes);
	return strikes_counter;
}

int main() {
	// INPUT
	int n, q;
	assert(scanf("%d %d", &n, &q));
	// input shapes
	Shape *shapes = (Shape*)malloc((size_t)n * sizeof(Shape));
	for(int i = 0; i < n; ++i) {
		input_shape(&shapes[i]);
	}
	// input and answer queries
	for(int i = 0; i < q; ++i) {
		int k; Vector2 p;
		assert(scanf("%d", &k));
		input_vector(&p);
		printf("%d\n", calculate_query(k - 1, p, shapes));
	}
	return 0;
}