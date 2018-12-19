package vis

import java.awt.Color
import java.awt.Graphics2D
import java.awt.Stroke

inline fun Graphics2D.drawTranslated(x: Int, y: Int, block: Graphics2D.() -> Unit) {
    val transform = transform
    translate(x, y)
    block()
    setTransform(transform)
}

inline fun Graphics2D.drawClipped(x: Int, y: Int, width: Int, height: Int, block: Graphics2D.() -> Unit) {
    val clip = clip
    clipRect(x, y, width, height)
    block()
    setClip(clip)
}

inline fun Graphics2D.drawWithStroke(stroke: Stroke, block: Graphics2D.() -> Unit) {
    val oldStroke = this.stroke
    this.stroke = stroke
    block()
    setStroke(oldStroke)
}

inline fun Graphics2D.drawWithColor(color: Color, block: Graphics2D.() -> Unit) {
    val oldColor = this.color
    this.color = color
    block()
    setColor(oldColor)
}
