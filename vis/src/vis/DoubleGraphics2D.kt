package vis

import java.awt.Color
import java.awt.Graphics2D
import java.awt.Stroke
import java.awt.geom.Arc2D
import java.awt.geom.Ellipse2D
import java.awt.geom.Line2D
import java.awt.geom.Rectangle2D
import java.util.*
import kotlin.math.roundToInt

class DoubleGraphics2D(val g: Graphics2D) {
    private val strokes = ArrayDeque<Stroke>()

    fun setColor(string: String) {
        when (string) {
            "white" -> setColor(255, 255, 255)
            "red" -> setColor(192, 16, 16)
            "green" -> setColor(16, 192, 16)
            "blue" -> setColor(16, 16, 192)
            "yellow" -> setColor(192, 192, 16)
        }
    }

    fun setColor(red: Int, green: Int, blue: Int) {
        g.color = Color(red, green, blue)
    }

    fun drawRect(x: Double, y: Double, width: Double, height: Double) {
        g.draw(Rectangle2D.Double(x, y, width, height))
    }

    fun fillRect(x: Double, y: Double, width: Double, height: Double) {
        g.fill(Rectangle2D.Double(x, y, width, height))
    }

    fun drawPolygon(xPoints: DoubleArray, yPoints: DoubleArray, nPoints: Int) {
        g.drawPolygon(
            IntArray(nPoints) { i -> xPoints[i].roundToInt() },
            IntArray(nPoints) { i -> yPoints[i].roundToInt() },
            nPoints
        )
    }

    fun fillPolygon(xPoints: DoubleArray, yPoints: DoubleArray, nPoints: Int) {
        g.fillPolygon(
            IntArray(nPoints) { i -> xPoints[i].roundToInt() },
            IntArray(nPoints) { i -> yPoints[i].roundToInt() },
            nPoints
        )
    }

    fun drawEllipse(x: Double, y: Double, width: Double, height: Double) {
        g.draw(Ellipse2D.Double(x, y, width, height))
    }

    fun fillEllipse(x: Double, y: Double, width: Double, height: Double) {
        g.fill(Ellipse2D.Double(x, y, width, height))
    }

    fun drawCircle(x: Double, y: Double, radius: Double) {
        drawEllipse(x - radius, y - radius, radius + radius, radius + radius)
    }

    fun fillCircle(x: Double, y: Double, radius: Double) {
        fillEllipse(x - radius, y - radius, radius + radius, radius + radius)
    }

    @JvmOverloads
    fun drawArc(x: Double, y: Double, radius: Double, startAngle: Double, endAngle: Double, type: Int = Arc2D.OPEN) {
        g.draw(Arc2D.Double(x - radius, y - radius, radius + radius, radius + radius, startAngle, endAngle, type))
    }

    @JvmOverloads
    fun fillArc(x: Double, y: Double, radius: Double, startAngle: Double, endAngle: Double, type: Int = Arc2D.OPEN) {
        g.fill(Arc2D.Double(x - radius, y - radius, radius + radius, radius + radius, startAngle, endAngle, type))
    }

    fun drawLine(x1: Double, y1: Double, x2: Double, y2: Double) {
        g.draw(Line2D.Double(x1, y1, x2, y2))
    }

    fun drawString(string: String, x: Double, y: Double) {
        g.drawString(string, x.toFloat(), y.toFloat())
    }

    fun pushStroke(stroke: Stroke) {
        strokes.push(g.stroke)
        g.stroke = stroke
    }

    fun popStroke() {
        g.stroke = strokes.poll()
    }
}
