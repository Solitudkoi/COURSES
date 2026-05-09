#include <iomanip>
#include <iostream>
#include <fstream>
#include <vector>

// C-style struct for a 2D point
struct Point {
  double x;
  double y;
};

// C-style struct for a line segment defined by two point indices
struct Line {
  int p1_index;
  int p2_index;
};

/**
 * @brief Reads points and lines data from the input file "in.data".
 * @return A pair of vectors: the first vector contains Point structs, and the
 * second vector contains Line structs.
 */
std::pair<std::vector<Point>, std::vector<Line>>
read_data() {
  std::ifstream infile("in.data");
  if (!infile.is_open()) {
    std::cerr << "Error: Could not open the input file." << std::endl;
    return {{}, {}};
  }

  int num_points, num_lines;
  infile >> num_points >> num_lines;

  std::vector<Point> points(num_points);
  std::vector<Line> lines;

  // [todo] Implement reading point data
  for (int i = 0; i < num_points; ++i) {
      Point p;
      infile >> p.x >> p.y; //流提取运算符（>>）从输入文件中读取点的 x 和 y 坐标，并将它们存储在 Point 结构体的成员变量中。
      points[i] = p;
  }

  // [todo] Implement reading line data
  for (int i = 0; i < num_lines; ++i) {
    Line l;
    infile >> l.p1_index >> l.p2_index;
    lines.push_back(l);
  }

  return {points, lines};
}

/**
 * @brief Prints the points and lines in a formatted table to the console.
 * @param points A vector of Point structs.
 * @param lines A vector of Line structs.
 */
void print_table(const std::vector<Point> &points, const std::vector<Line> &lines) {
    // Points table
    std::cout << "+------------------------------------+" << std::endl;
    std::cout << "|               Points               |" << std::endl;
    std::cout << "+--------+-------------+-------------+" << std::endl;
    std::cout << "| Index  |      X      |      Y      |" << std::endl;
    std::cout << "+--------+-------------+-------------+" << std::endl;
    for (size_t i = 0; i < points.size(); ++i) {
        std::cout << "| "         //使用 std 命名空间里的 cout 对象来输出格式化的表格行。
                  << std::right << std::setw(6) << i //std::right 用于右对齐，std::setw 设置字段宽度，
                  << " | " 
                  << std::right << std::setw(11) << std::fixed << std::setprecision(1) << points[i].x 
                  << " | " 
                  << std::right << std::setw(11) << std::fixed << std::setprecision(1) << points[i].y 
                  << " |" << std::endl;
        //std::fixed 和 std::setprecision(1) 用于控制浮点数的显示格式，使其显示为固定小数点格式并保留一位小数。
        //如果文件开头添加了 using namespace std; 则可以省略 std:: 前缀，但在大型项目中建议避免使用 using namespace std; 以防止命名冲突。
    }
    std::cout << "+--------+-------------+-------------+" << std::endl;

    std::cout << std::endl;

    // Lines table
    std::cout << "+------------------------------------+" << std::endl;
    std::cout << "|               Lines                |" << std::endl;
    std::cout << "+--------+-------------+-------------+" << std::endl;
    std::cout << "| Index  |   Point 1   |   Point 2   |" << std::endl;
    std::cout << "+--------+-------------+-------------+" << std::endl;
    for (size_t i = 0; i < lines.size(); ++i) {
        std::cout << "| " 
                  << std::right << std::setw(6) << i 
                  << " | " 
                  << std::right << std::setw(11) << lines[i].p1_index 
                  << " | " 
                  << std::right << std::setw(11) << lines[i].p2_index 
                  << " |" << std::endl;
    }
    std::cout << "+--------+-------------+-------------+" << std::endl;
}

/**
 * @brief Writes the points and lines to an SVG file named "out.svg".
 * @param points A vector of Point structs.
 * @param lines A vector of Line structs.
 */
void write_svg(const std::vector<Point> &points, const std::vector<Line> &lines) {
    if (points.empty()) return;

    // Calculate bounding box
    double min_x = points[0].x, max_x = points[0].x;
    double min_y = points[0].y, max_y = points[0].y;
    for (const auto& p : points) {
        min_x = std::min(min_x, p.x);
        max_x = std::max(max_x, p.x);
        min_y = std::min(min_y, p.y);
        max_y = std::max(max_y, p.y);
    }

    double padding = 10.0;
    double vb_x = min_x - padding;
    double vb_y = min_y - padding;
    double vb_width = max_x - min_x + 2 * padding;
    double vb_height = max_y - min_y + 2 * padding;

    std::ofstream outfile("out.svg");
    if (!outfile) {
        std::cerr << "Unable to create file out.svg" << std::endl;
        return;
    }

    // SVG header
    outfile << "<svg width=\"" << std::fixed << std::setprecision(1) << vb_width << "\" height=\"" << vb_height << "\" viewBox=\"" << vb_x << " " << vb_y << " " << vb_width << " " << vb_height << "\" xmlns=\"http://www.w3.org/2000/svg\">" << std::endl;

    // Draw lines
    for (const auto &line : lines) {
        const Point &p1 = points[line.p1_index];
        const Point &p2 = points[line.p2_index];
        outfile << "  <line x1=\"" << p1.x << "\" y1=\"" << p1.y 
                << "\" x2=\"" << p2.x << "\" y2=\"" << p2.y 
                << "\" stroke=\"black\" stroke-width=\"2.0\"/>" << std::endl;
    }

    // Draw points
    for (const auto &point : points) {
        outfile << "  <circle cx=\"" << point.x << "\" cy=\"" << point.y 
                << "\" r=\"4.0\" fill=\"black\"/>" << std::endl;
    }

    // SVG footer
    outfile << "</svg>" << std::endl;
}
