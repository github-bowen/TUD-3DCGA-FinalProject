// bezier_curve.cpp
#include "bezier_curve.h"

BezierCurve::BezierCurve() {
	setupPath();
}

BezierCurve::BezierCurve(bool showBezierPath, float time) : showBezierPath(showBezierPath), time(time), segments() {
    setupPath();
}

// initialize path with 3 segments
void BezierCurve::setupPath() {
	addSegment(glm::vec3(-4.0f, 2.0f, 0.0f), glm::vec3(-6.0f, 4.5f, 6.0f), glm::vec3(3.5f, 6.5f, 2.0f), glm::vec3(6.0f, 5.0f, -5.0f));
	addSegment(glm::vec3(4.0f, 5.0f, -5.0f), glm::vec3(7.0f, 2.5f, 0.0f), glm::vec3(5.5f, -2.5f, 4.0f), glm::vec3(-3.0f, -4.0f, 8.0f));
	addSegment(glm::vec3(-3.0f, -4.0f, 8.0f), glm::vec3(5.5f, 2.5f, -3.0f), glm::vec3(-7.5f, 7.5f, -6.0f), glm::vec3(-4.0f, 2.0f, 0.0f));
}

// add one segment of cubic Bezier Curve 
void BezierCurve::addSegment(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3) {
    segments.push_back({ p0, p1, p2, p3 });
	updateArcLengthTable();  // NOTE: added for moving at constant speed
}

// set control points of specified segment
void BezierCurve::setSegmentControlPoints(size_t segmentIndex, const glm::vec3& p0, 
    const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3) {
    if (segmentIndex < segments.size()) {
        segments[segmentIndex] = { p0, p1, p2, p3 };
    } else {
		throw std::out_of_range("Error in BezierCurve::setSegmentControlPoints: \nSegment index out of range! Given index: "
			+ std::to_string(segmentIndex) + ", Segment count: " + std::to_string(segments.size()));
    }
}

glm::vec3 BezierCurve::getCurrentPointOnPath() const {
	return getPointOnPath(this->time);
}

// get point on path at given time (time belongs to [0, 1])
glm::vec3 BezierCurve::getPointOnPath(float anotherTime) const {
    if (segments.empty()) return glm::vec3(0.0f);  // return 0 vector if no segments

    // calculate current segment index at given time
    float segmentDuration = 1.0f / segments.size();  // duration of each segment
    int segmentIndex = std::min(static_cast<int>(anotherTime * segments.size()), static_cast<int>(segments.size() - 1));
    float t = (anotherTime - segmentIndex * segmentDuration) / segmentDuration; // local t value for current segment

    // get control points of current segment and calculate point
    const std::vector<glm::vec3>& controlPoints = segments[segmentIndex];
    return calculateCubicBezierPoint(t, controlPoints[0], controlPoints[1], controlPoints[2], controlPoints[3]);
}

// return total segment count
size_t BezierCurve::getSegmentCount() const {
    return segments.size();
}

// get control points of specified segment
std::vector<glm::vec3> BezierCurve::getSegmentControlPoints(size_t segmentIndex) const {
    if (segmentIndex < segments.size()) {
        return segments[segmentIndex];
    } else {
        throw std::out_of_range("Error in BezierCurve::getSegmentControlPoints: \nSegment index out of range! Given index: "
            + std::to_string(segmentIndex) + ", Segment count: " + std::to_string(segments.size()));
    }
}

// calculate point of cubic Bezier curve
glm::vec3 BezierCurve::calculateCubicBezierPoint(float t, const glm::vec3& p0, const glm::vec3& p1, 
    const glm::vec3& p2, const glm::vec3& p3) const {

    float u = 1.0f - t;
    float tt = t * t;
    float uu = u * u;
    float uuu = uu * u;
    float ttt = tt * t;

    glm::vec3 point = uuu * p0;        // (1 - t)^3 * p0
    point += 3 * uu * t * p1;          // 3 * (1 - t)^2 * t * p1
    point += 3 * u * tt * p2;          // 3 * (1 - t) * t^2 * p2
    point += ttt * p3;                 // t^3 * p3

    return point;
}

// approximate arc length of cubic Bezier curve between t0 and t1
float BezierCurve::approximateArcLength(float t0, float t1, int segmentIndex, int samples) const {
    float arcLength = 0.0f;
    glm::vec3 previousPoint = calculateCubicBezierPoint(t0, segments[segmentIndex][0], 
        segments[segmentIndex][1], segments[segmentIndex][2], segments[segmentIndex][3]);

    for (int i = 1; i <= samples; ++i) {
        float t = t0 + (t1 - t0) * i / samples;
        glm::vec3 currentPoint = calculateCubicBezierPoint(t, segments[segmentIndex][0], segments[segmentIndex][1], 
            segments[segmentIndex][2], segments[segmentIndex][3]);
		arcLength += glm::distance(previousPoint, currentPoint);  // use line to approximate curve
        previousPoint = currentPoint;
    }
    return arcLength;
}

// update arc length table for each segment
void BezierCurve::updateArcLengthTable() {
    arcLengthTable.clear();
    float totalArcLength = 0.0f;
    const int samplesPerSegment = 50;

    for (size_t i = 0; i < segments.size(); ++i) {
        for (int j = 0; j < samplesPerSegment; ++j) {
            float t0 = static_cast<float>(j) / samplesPerSegment;
            float t1 = static_cast<float>(j + 1) / samplesPerSegment;
            float segmentArcLength = approximateArcLength(t0, t1, i);

            totalArcLength += segmentArcLength;
            arcLengthTable.push_back({ totalArcLength, i + t0 });
        }
    }
}

glm::vec3 BezierCurve::getPointAtCurrentDistance() const {
	return getPointAtDistance(this->distance);
}

// get point given distance by searching arc length table
glm::vec3 BezierCurve::getPointAtDistance(float distance) const {
    if (arcLengthTable.empty() || distance <= 0.0f) {
        return getPointOnPath(0.0f);
    }
    if (distance >= arcLengthTable.back().arcLength) {
        return getPointOnPath(1.0f);
    }

	// *binary search* to find the given distance
    size_t low = 0, high = arcLengthTable.size() - 1;
    while (low < high) {
        size_t mid = (low + high) / 2;
		float midArcLength = arcLengthTable[mid].arcLength;

        if (midArcLength == distance) {
			low = mid;
			break;
        } else if (midArcLength < distance) {
            low = mid + 1;
        } else {
            high = mid;
        }
    }

    float t = arcLengthTable[low].time;
    return getPointOnPath(t);
}
