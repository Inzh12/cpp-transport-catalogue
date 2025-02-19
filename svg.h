#pragma once

#include <cstdint>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <optional>
#include <variant>

namespace svg {

struct Rgb {
    uint8_t red = 0;
    uint8_t green = 0;
    uint8_t blue = 0;
};

struct Rgba {
    uint8_t red = 0;
    uint8_t green = 0;
    uint8_t blue = 0;
    double opacity = 1;
};

using Color = std::variant<std::monostate, std::string, Rgb, Rgba>;
inline const Color NoneColor{std::monostate{}};

struct ColorPrinter {
    std::ostream& out;

    void operator()(std::monostate) const;
    void operator()(const std::string& str) const;
    void operator()(Rgb rbg) const;
    void operator()(Rgba rbga) const;
};

std::ostream& operator<<(std::ostream& out, Color color);

enum class StrokeLineCap {
    BUTT,
    ROUND,
    SQUARE,
};

enum class StrokeLineJoin {
    ARCS,
    BEVEL,
    MITER,
    MITER_CLIP,
    ROUND,
};

std::ostream& operator<<(std::ostream& out, StrokeLineCap stroke_line_cap);
std::ostream& operator<<(std::ostream& out, StrokeLineJoin stroke_line_join);

struct Point {
    Point() = default;
    Point(double x, double y)
        : x(x)
        , y(y) {
    }
    double x = 0;
    double y = 0;
};

/*
 * Вспомогательная структура, хранящая контекст для вывода SVG-документа с отступами.
 * Хранит ссылку на поток вывода, текущее значение и шаг отступа при выводе элемента
 */
struct RenderContext {
    RenderContext(std::ostream& out)
        : out(out) {
    }

    RenderContext(std::ostream& out, int indent_step, int indent = 0)
        : out(out)
        , indent_step(indent_step)
        , indent(indent) {
    }

    RenderContext Indented() const {
        return {out, indent_step, indent + indent_step};
    }

    void RenderIndent() const {
        for (int i = 0; i < indent; ++i) {
            out.put(' ');
        }
    }

    std::ostream& out;
    int indent_step = 0;
    int indent = 0;
};


/*
 * Абстрактный базовый класс Object служит для унифицированного хранения
 * конкретных тегов SVG-документа
 * Реализует паттерн "Шаблонный метод" для вывода содержимого тега
 */
class Object {
public:
    void Render(const RenderContext& context) const;

    virtual ~Object() = default;

private:
    virtual void RenderObject(const RenderContext& context) const = 0;
};

template <typename Owner>
class PathProps {
public:
    Owner& SetFillColor(Color color) {
        fill_color_ = std::move(color);
        return AsOwner();
    }

    Owner& SetStrokeColor(Color color) {
        stroke_color_ = std::move(color);
        return AsOwner();
    }

    Owner& SetStrokeWidth(double width) {
        stroke_width_ = width;
        return AsOwner();
    }

    Owner& SetStrokeLineCap(StrokeLineCap line_cap) {
        stroke_line_cap_ = line_cap;
        return AsOwner();
    }

    Owner& SetStrokeLineJoin(StrokeLineJoin line_join) {
        stroke_line_join_ = line_join;
        return AsOwner();
    }
protected:
    ~PathProps() = default;
    void RenderAttrs(std::ostream& out) const {
        using namespace std::literals;

        if (fill_color_) {
            out << " fill=\""sv << *fill_color_ << "\""sv;
        }
        if (stroke_color_) {
            out << " stroke=\""sv << *stroke_color_ << "\""sv;
        }
        if (stroke_width_) {
            out << " stroke-width=\"" << *stroke_width_ << "\""sv;
        }
        if (stroke_line_cap_) {
            out << " stroke-linecap=\"" << *stroke_line_cap_ << "\"";
        }
        if (stroke_line_join_) {
            out << " stroke-linejoin=\"" << *stroke_line_join_ << "\"";
        }
    }
private:
    Owner& AsOwner() {
        return static_cast<Owner&>(*this);
    }

    std::optional<Color> fill_color_;
    std::optional<Color> stroke_color_;
    std::optional<double> stroke_width_;
    std::optional<StrokeLineCap> stroke_line_cap_;
    std::optional<StrokeLineJoin> stroke_line_join_;
};

/*
 * Класс Circle моделирует элемент <circle> для отображения круга
 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/circle
 */
class Circle final : public Object, public PathProps<Circle> {
public:
    Circle& SetCenter(Point center);
    Circle& SetRadius(double radius);

private:
    void RenderObject(const RenderContext& context) const override;

    Point center_;
    double radius_ = 1.0;
};

/*
 * Класс Polyline моделирует элемент <polyline> для отображения ломаных линий
 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/polyline
 */
class Polyline final : public Object, public PathProps<Polyline> {
public:
    Polyline& AddPoint(Point point);
private:
    void RenderObject(const RenderContext& context) const override;

    std::vector<Point> points_;
};

/*
 * Класс Text моделирует элемент <text> для отображения текста
 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/text
 */
class Text final : public Object, public PathProps<Text> {
public:
    // Задаёт координаты опорной точки (атрибуты x и y)
    Text& SetPosition(Point pos);

    // Задаёт смещение относительно опорной точки (атрибуты dx, dy)
    Text& SetOffset(Point offset);

    // Задаёт размеры шрифта (атрибут font-size)
    Text& SetFontSize(uint32_t size);

    // Задаёт название шрифта (атрибут font-family)
    Text& SetFontFamily(std::string font_family);

    // Задаёт толщину шрифта (атрибут font-weight)
    Text& SetFontWeight(std::string font_weight);

    // Задаёт текстовое содержимое объекта (отображается внутри тега text)
    Text& SetData(std::string data);
private:
    void RenderObject(const RenderContext& context) const override;

    std::string Escape(std::string_view str) const;

    Point pos_ = {0, 0};
    Point offset_ = {0, 0};
    uint32_t size_ = 1;
    std::string font_family_;
    std::string font_weight_;
    std::string data_;
};

class ObjectContainer {
public:
    template <typename T>
    void Add(T obj);

    virtual void AddPtr(std::unique_ptr<Object>&& ptr) = 0;

    virtual ~ObjectContainer();
};

template <typename T>
void ObjectContainer::Add(T obj) {
    AddPtr(std::make_unique<T>(std::move(obj)));
}

class Document : public ObjectContainer {
public:
    void AddPtr(std::unique_ptr<Object>&& obj) override;

    void Render(std::ostream& out) const;
private:
    std::vector<std::unique_ptr<Object>> objects_;
};

class Drawable {
public:
    virtual void Draw(ObjectContainer& container) const = 0;

    virtual ~Drawable();
};

namespace shapes {

class Triangle : public Drawable {
public:
    Triangle(Point p1, Point p2, Point p3);
    void Draw(ObjectContainer& container) const override;
private:
    Point p1_;
    Point p2_;
    Point p3_;
};

class Star : public Drawable {
public:
    Star(Point center, double outer_radius, double inner_radius, int num_rays);
    void Draw(ObjectContainer& container) const override;
private:
    Point center_;
    double outer_radius_;
    double inner_radius_;
    int num_rays_;
};

class Snowman : public Drawable {
public:
    Snowman(Point head_center, double head_radius);
    void Draw(ObjectContainer& container) const override;
private:
    Point head_center_;
    double head_radius_;
};

} // namespace shapes

}  // namespace svg
