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

	// get point on path at given time (this->time belongs to [0, 1])
    glm::vec3 getPointOnPath() const;

	// get total segment count
    size_t getSegmentCount() const;

	// get control points of specified segment
    std::vector<glm::vec3> getSegmentControlPoints(size_t segmentIndex) const;

    // control points of each segment
    std::vector<std::vector<glm::vec3>> segments;

    /* ------------------ PUBLIC VARIABLES: UI COMPONENTS ------------------ */
    bool showBezierPath;
	float time;  // time value for path point calculation. NOTE: different from t value in each segment



private:

	// calculate point of cubic Bezier curve
    glm::vec3 calculateCubicBezierPoint(float t, const glm::vec3& p0, 
        const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3) const;

	void setupPath();
};

#endif // BEZIER_CURVE_H
