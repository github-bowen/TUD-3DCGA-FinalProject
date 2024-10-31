// bezier_curve.h
#ifndef BEZIER_CURVE_H
#define BEZIER_CURVE_H

#include <glm/glm.hpp>
#include <vector>
#include <stdexcept>
#include <string>

/*
 * BezierCurve: 
 * Class representing a path composed of serveral segments of cubic Bezier curves
 */
class BezierCurve {
public:
    BezierCurve();

    BezierCurve(bool showBezierPath, float time);

    // add one segment of cubic Bezier Curve 
    void addSegment(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3);

	// set control points of specified segment
    void setSegmentControlPoints(size_t segmentIndex, const glm::vec3& p0, 
        const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3);

	// get point on path at current time: this->time
    glm::vec3 getCurrentPointOnPath() const;

	// get point on path at given time (time belongs to [0, 1])
	glm::vec3 getPointOnPath(float anotherTime) const;

	// get total segment count
    size_t getSegmentCount() const;

	// get control points of specified segment
    std::vector<glm::vec3> getSegmentControlPoints(size_t segmentIndex) const;

    // control points of each segment
    std::vector<std::vector<glm::vec3>> segments;

    /* ------------------ EXTRA PART: Moving At Constant Speed ------------------ */

	// get point on path at given distance when moving at constant speed
    glm::vec3 getPointAtDistance(float distance) const;

    glm::vec3 getPointAtCurrentDistance() const;

	// update arc length table for each segment
    void updateArcLengthTable();

    /* ------------------ PUBLIC VARIABLES: UI COMPONENTS ------------------ */
    bool showBezierPath = false;
	float time = 0.0f;  // current time value. NOTE: different from t value in each segment

	float distance = 0.0f; // current distance value
    float speed = 0.05f;
    bool movingAtConstantSpeed = true;
	int direction = 1;  // 1: forward, -1: backward

private:

	// calculate point of cubic Bezier curve
    glm::vec3 calculateCubicBezierPoint(float t, const glm::vec3& p0, 
        const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3) const;

	void setupPath();

    /* ------------------ EXTRA PART: Moving At Constant Speed ------------------ */

    struct ArcLengthEntry {
        float arcLength;
		float time;      // corresponding time value
    };

	std::vector<ArcLengthEntry> arcLengthTable; // arc length table for each segment

	// approximate arc length of cubic Bezier curve between t0 and t1
    float approximateArcLength(float t0, float t1, int segmentIndex, int samples = 10) const;
};

#endif // BEZIER_CURVE_H
