package vis.test

import vis.StepData
import vis.drawWithColor
import vis.drawWithStroke
import java.awt.BasicStroke
import java.awt.Color
import java.util.function.Consumer

fun createTestGameStates(addState: (Int, StepData) -> Unit) {
    for (x in 0..1000) {
        val header = listOf(
            "step $x",
            "score 1 = ...",
            "score 2 = ..."
        )
        val text = listOf(
            "Lorem ipsum dolor sit amet",
            "consectetur adipiscing elit",
            "sed do eiusmod tempor incididunt ut labore et dolore magna aliqua",
            "...",
            "   beep blop",
            "",
            "",
            "000sf00f",
            "by the way, step number is $x",
            "",
            "",
            "¯\\_(ツ)_/¯"
        )
        val state = StepData(header, text, drawWorld = Consumer { g ->
            with(g) {
                drawWithColor(Color.DARK_GRAY) {
                    drawLine(0, -10000, 0, 10000)
                    drawLine(-10000, 0, 10000, 0)
                }

                drawWithStroke(BasicStroke(4.0f)) {
                    drawWithColor(Color.RED) {
                        drawArc(100 + x, 100, 100, 100, 0, 360)
                        drawLine(150 + x, 200, 150 + x, 280)
                        drawLine(130 + x, 240, 170 + x, 240)
                    }

                    drawWithColor(Color.BLUE) {
                        fillArc(2100 - x, 100, 100, 100, 0, 360)
                        drawLine(2150 - x, 150, 2230 - x, 70)
                        drawLine(2230 - x, 70, 2200 - x, 70)
                        drawLine(2230 - x, 70, 2230 - x, 100)
                    }
                }
            }
        }, drawExtras = Consumer {
            // Do nothing
        })
        addState(x, state)
    }
}
