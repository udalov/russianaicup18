package vis

import java.awt.Graphics2D
import java.io.File
import java.util.function.Consumer
import kotlin.math.roundToInt

private fun Graphics2D.drawActions(actions: List<String>) {
    val g = DoubleGraphics2D(this)
    for (action in actions) {
        val ss = action.split(' ')
        val p0 = ss.getOrNull(0)
        val p1 = ss.getOrNull(1)
        val n1 = ss.getOrNull(1)?.toDoubleOrNull()
        val n2 = ss.getOrNull(2)?.toDoubleOrNull()
        val n3 = ss.getOrNull(3)?.toDoubleOrNull()
        val n4 = ss.getOrNull(4)?.toDoubleOrNull()
        val n5 = ss.getOrNull(5)?.toDoubleOrNull()

        val draw = p0 != null && p0.startsWith("d")
        when (p0) {
            "color" -> {
                if (p1 != null && p1.isNotEmpty() && p1[0].isLetter()) {
                    g.setColor(p1)
                } else if (n1 != null && n2 != null && n3 != null) {
                    g.setColor(n1.roundToInt(), n2.roundToInt(), n3.roundToInt())
                }
            }
            "draw-rect", "fill-rect" -> if (n1 != null && n2 != null && n3 != null && n4 != null) {
                if (draw) {
                    g.drawRect(n1, n2, n3, n4)
                } else {
                    g.fillRect(n1, n2, n3, n4)
                }
            }
            "draw-poly", "fill-poly" -> {
                val coords = ss.subList(1, ss.size).mapNotNull(String::toDoubleOrNull)
                val n = coords.size / 2
                if (coords.size == n * 2) {
                    val xs = DoubleArray(n) { i -> coords[i * 2] }
                    val ys = DoubleArray(n) { i -> coords[i * 2 + 1] }
                    if (draw) {
                        g.drawPolygon(xs, ys, n)
                    } else {
                        g.fillPolygon(xs, ys, n)
                    }
                }
            }
            "draw-circle", "fill-circle" -> if (n1 != null && n2 != null && n3 != null) {
                if (draw) {
                    g.drawCircle(n1, n2, n3)
                } else {
                    g.fillCircle(n1, n2, n3)
                }
            }
            "draw-arc", "fill-arc" -> if (n1 != null && n2 != null && n3 != null && n4 != null && n5 != null) {
                if (draw) {
                    g.drawArc(n1, n2, n3, n4, n5)
                } else {
                    g.fillArc(n1, n2, n3, n4, n5)
                }
            }
            "draw-line" -> if (n1 != null && n2 != null && n3 != null && n4 != null) {
                g.drawLine(n1, n2, n3, n4)
            }
            "string" -> if (n1 != null && n2 != null) {
                g.drawString(ss.subList(3, ss.size).joinToString(" "), n1, n2)
            }
        }
    }
}

private enum class FileReadState {
    BOARD,
    STATE,
    EXTRAS,
    HEADER,
    TEXTAREA
}

internal fun readStepsFromFile(file: File, addStepData: (Int, StepData) -> Unit) {
    var st = FileReadState.BOARD
    val board = arrayListOf<String>()
    val actions = arrayListOf<String>()
    val extras = arrayListOf<String>()
    val header = arrayListOf<String>()
    val textarea = arrayListOf<String>()
    var currentStepNumber = 0
    val drawBoard: Graphics2D.() -> Unit = { drawActions(board) }
    val handleLine: (String) -> Unit = { line ->
        when (line) {
            "---" -> {
                if (st != FileReadState.BOARD) {
                    val actionsToDraw = actions.toList()
                    val extrasToDraw = extras.toList()
                    addStepData(currentStepNumber++, StepData(header.toList(), textarea.toList(), drawWorld = Consumer {
                        it.drawBoard()
                        it.drawActions(actionsToDraw)
                    }, drawExtras = Consumer {
                        it.drawActions(extrasToDraw)
                    }))
                    actions.clear()
                    extras.clear()
                    header.clear()
                    textarea.clear()
                }
                st = FileReadState.STATE
            }
            ">" -> if (st == FileReadState.STATE || st == FileReadState.EXTRAS) {
                st = FileReadState.HEADER
            }
            "-" -> if (st == FileReadState.HEADER) {
                st = FileReadState.TEXTAREA
            }
            "%" -> if (st == FileReadState.STATE) {
                st = FileReadState.EXTRAS
            }
            else -> when (st) {
                FileReadState.BOARD -> board.add(line)
                FileReadState.STATE -> actions.add(line)
                FileReadState.EXTRAS -> extras.add(line)
                FileReadState.HEADER -> header.add(line)
                FileReadState.TEXTAREA -> textarea.add(line)
            }
        }
    }
    file.forEachLine(action = handleLine)
    if (actions.isNotEmpty() || extras.isNotEmpty() || header.isNotEmpty() || textarea.isNotEmpty()) {
        handleLine("---")
    }
}
