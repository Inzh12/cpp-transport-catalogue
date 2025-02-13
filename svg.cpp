#define _USE_MATH_DEFINES
#include "svg.h"
#include <cmath>

namespace svg {

using namespace std::literals;

std::ostream& operator<<(std::ostream& out, StrokeLineCap stroke_line_cap) {
    switch (stroke_line_cap) {
    case StrokeLineCap::BUTT:
        out << "butt";
        break;
    case StrokeLineCap::ROUND:
        out << "round";
        break;
    case StrokeLineCap::SQUARE:
        out << "square";
    }

    return out;
}

std::ostream& operator<<(std::ostream& out, StrokeLineJoin stroke_line_join) {
    switch (stroke_line_join) {
    case StrokeLineJoin::ARCS:
        out << "arcs";
        break;
    case StrokeLineJoin::BEVEL:
        out << "bevel";
        break;
    case StrokeLineJoin::MITER:
        out << "miter";
        break;
    case StrokeLineJoin::MITER_CLIP:
        out << "miter-clip";
        break;
    case StrokeLineJoin::ROUND:
        out << "round";
        break;
    }

    return out;
}


// ---------- Object ------------------

void Object::Render(const RenderContext& context) const {
    context.RenderIndent();

    // Делегируем вывод тега своим подклассам
    RenderObject(context);

    context.out << std::endl;
}

// ---------- ObjectContainer ------------------

ObjectContainer::~ObjectContainer() {}

// ---------- Document ------------------

void Document::AddPtr(std::unique_ptr<Object> &&obj) {
    objects_.push_back(std::move(obj));
}

void Document::Render(std::ostream &out) const {
    RenderContext context {out, 2, 2};

    out << R"(<?xml version="1.0" encoding="UTF-8" ?>)"sv << '\n';
    out << R"(<svg xmlns="http://www.w3.org/2000/svg" version="1.1">)"sv << '\n';

    for (const auto& obj_ptr : objects_) {
        obj_ptr->Render(context);
    }

    out << "</svg>"sv;
}

// ---------- Circle ------------------

Circle& Circle::SetCenter(Point center)  {
    center_ = center;
    return *this;
}

Circle& Circle::SetRadius(double radius)  {
    radius_ = radius;
    return *this;
}

void Circle::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv;
    out << "r=\""sv << radius_ << "\""sv;
    RenderAttrs(context.out);
    out << "/>"sv;
}

// ---------- Polyline ------------------

Polyline &Polyline::AddPoint(Point point) {
    points_.push_back(point);
    return *this;
}

void Polyline::RenderObject(const RenderContext &context) const {
    auto& out = context.out;
    out << "<polyline points=\""sv;

    if (!points_.empty()) {
        const Point* point = &points_.at(0);
        out << point->x << ',' << point->y;
        for (int i = 1; i < static_cast<int>(points_.size()); ++i) {
            point = &points_.at(i);

            out << ' ' << point->x << ',' << point->y;
        }
    }

    out << '"';
    RenderAttrs(context.out);
    out << "/>"sv;
}

// ---------- Text ------------------

Text &Text::SetPosition(Point pos) {
    pos_ = pos;
    return *this;
}

Text &Text::SetOffset(Point offset) {
    offset_ = offset;
    return *this;
}

Text &Text::SetFontSize(uint32_t size) {
    size_ = size;
    return *this;
}

Text &Text::SetFontFamily(std::string font_family) {
    font_family_ = font_family;
    return *this;
}

Text &Text::SetFontWeight(std::string font_weight)
{
    font_weight_ = font_weight;
    return *this;
}

Text &Text::SetData(std::string data)
{
    data_ = data;
    return *this;
}

void Text::RenderObject(const RenderContext &context) const {
    auto& out = context.out;

    out << "<text";

    RenderAttrs(context.out);

    out << R"( x=")"sv << pos_.x << '"'
        << R"( y=")"sv << pos_.y << '"'
        << R"( dx=")"sv << offset_.x << '"'
        << R"( dy=")"sv << offset_.y << '"'
        << R"( font-size=")"sv << size_ << '"';

    if (!font_family_.empty()) {
        out << R"( font-family=")"sv << font_family_ << '"';
    }

    if (!font_weight_.empty()) {
        out << R"( font-weight=")"sv << font_weight_ << '"';
    }

    out << '>';

    if (!data_.empty()) {
        out << Escape(data_);
    }

    out << "</text>"sv;
}

std::string Text::Escape(std::string_view str) const {
    std::string result;

    for (char symbol : str) {
        switch (symbol) {
        case '"':
            result.append("&quot;"sv);
            break;
        case '\'':
            result.append("&apos;"sv);
            break;
        case '<':
            result.append("&lt;"sv);
            break;
        case '>':
            result.append("&gt;"sv);
            break;
        case '&':
            result.append("&amp;"sv);
            break;
        default:
            result.push_back(symbol);
            break;
        }
    }

    return result;
}

// ---------- Drawable ------------------

Drawable::~Drawable() {}

namespace shapes {

// ---------- Triangle ------------------

Triangle::Triangle(Point p1, Point p2, Point p3)
    : p1_(p1), p2_(p2), p3_(p3) {}

void Triangle::Draw(ObjectContainer &container) const
{
    container.Add(Polyline().AddPoint(p1_).AddPoint(p2_).AddPoint(p3_).AddPoint(p1_));
}

// ---------- Star ------------------

Star::Star(Point center, double outer_radius, double inner_radius, int num_rays)
    : center_(center)
    , outer_radius_(outer_radius)
    , inner_radius_(inner_radius)
    , num_rays_(num_rays) {}

void Star::Draw(ObjectContainer &container) const {
    Polyline polyline;
    for (int i = 0; i <= num_rays_; ++i) {
        double angle = 2 * M_PI * (i % num_rays_) / num_rays_;
        polyline.AddPoint({
            center_.x + outer_radius_ * sin(angle),
            center_.y - outer_radius_ * cos(angle)
        });
        if (i == num_rays_) {
            break;
        }
        angle += M_PI / num_rays_;
        polyline.AddPoint({
            center_.x + inner_radius_ * sin(angle),
            center_.y - inner_radius_ * cos(angle)
        });
    }
    polyline.SetFillColor("red").SetStrokeColor("black");
    container.Add(polyline);
}

// ---------- Snowman ------------------

Snowman::Snowman(Point head_center, double head_radius)
    : head_center_(head_center)
    , head_radius_(head_radius) {}

void Snowman::Draw(ObjectContainer &container) const {
    Circle head = Circle()
        .SetCenter(head_center_)
        .SetRadius(head_radius_)
        .SetFillColor("rgb(240,240,240)")
        .SetStrokeColor("black");

    Circle body = Circle()
        .SetCenter({head_center_.x, head_center_.y + 2 * head_radius_})
        .SetRadius(1.5 * head_radius_)
        .SetFillColor("rgb(240,240,240)")
        .SetStrokeColor("black");

    Circle feet = Circle()
        .SetCenter({head_center_.x, head_center_.y + 5 * head_radius_})
        .SetRadius(2 * head_radius_)
        .SetFillColor("rgb(240,240,240)")
        .SetStrokeColor("black");

    container.Add(feet);
    container.Add(body);
    container.Add(head);
}

} // namespace shapes

void ColorPrinter::operator()(std::monostate) const {
    out << "none"sv;
}

void ColorPrinter::operator()(const std::string& str) const {
    out << str;
}

void ColorPrinter::operator()(Rgb rgb) const {
    out << "rgb("sv
        << static_cast<int>(rgb.red) << ","sv
        << static_cast<int>(rgb.green) << ","sv
        << static_cast<int>(rgb.blue)
        << ")"sv;
}

void ColorPrinter::operator()(Rgba rgba) const {
    out << "rgba("sv
        << static_cast<int>(rgba.red) << ","sv
        << static_cast<int>(rgba.green) << ","sv
        << static_cast<int>(rgba.blue) << ","sv
        << rgba.opacity
        << ")"sv;
}

std::ostream& operator<<(std::ostream& out, Color color) {
    std::visit(ColorPrinter{out}, color);
    return out;
}

}  // namespace svg
