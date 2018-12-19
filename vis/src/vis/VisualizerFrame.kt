package vis

import java.awt.Dimension
import java.awt.Graphics
import java.awt.Graphics2D
import java.awt.event.ComponentEvent
import java.awt.event.ComponentListener
import javax.swing.JFrame
import javax.swing.JPanel
import kotlin.concurrent.thread

class VisualizerFrame(
    private val visuals: VisualState,
    private val data: GameData,
    updateAction: () -> Unit
) : JFrame() {
    init {
        defaultCloseOperation = EXIT_ON_CLOSE
        addKeyListener(VisKeyListener(visuals, data, updateAction, this::repaint))
        contentPane.preferredSize = Dimension(
            HEADER_LOCATION.x + HEADER_WIDTH + MARGIN,
            PROGRESS_LOCATION.y + PROGRESS_HEIGHT + FOOTER_FONT.size + MARGIN
        )
        background = BACKGROUND_COLOR
        contentPane.addComponentListener(object : ComponentListener {
            override fun componentMoved(e: ComponentEvent) = Unit

            override fun componentResized(e: ComponentEvent) {
                WORLD_WIDTH = contentPane.width - HEADER_WIDTH - 3 * MARGIN
                WORLD_HEIGHT = contentPane.height - PROGRESS_HEIGHT - FOOTER_FONT.size - 3 * MARGIN
            }

            override fun componentHidden(e: ComponentEvent) = Unit

            override fun componentShown(e: ComponentEvent) = Unit
        })
        contentPane.add(object : JPanel() {
            // val textArea: JTextArea

            init {
/*
                    textArea = object : JTextArea() {
                        init {
                            location = TEXTAREA_LOCATION
                            background = TEXTAREA_BACKGROUND_COLOR
                            foreground = TEXTAREA_FOREGROUND_COLOR
                            caretColor = TEXTAREA_FOREGROUND_COLOR
                            font = FONT
                            lineWrap = true
                            isEditable = false
                            isFocusable = false
                            isDoubleBuffered = true
                            setSize(TEXTAREA_WIDTH, TEXTAREA_HEIGHT)
                        }
                    }
                    contentPane.add(textArea)
*/
                addMouseListener(VisMouseListener(visuals, data, this::setCursor, this::repaint))
                addMouseMotionListener(VisMouseMotionListener(visuals, data, this::repaint))
                addMouseWheelListener(VisMouseWheelListener(visuals, this::repaint))

                background = BACKGROUND_COLOR
            }

            override fun paintComponent(g: Graphics) {
                super.paintComponent(g)
                with(g as Graphics2D) {
                    draw(visuals, data)
                }
/*
                    textArea.text = currentState?.text?.joinToString("\n")
                    textArea.repaint()
*/
            }
        })
        pack()
        isVisible = true
        thread(name = "autoPlay") {
            while (true) {
                while (!visuals.autoPlay) {
                    Thread.sleep(100L) // TODO
                }
                repaint()
                while (visuals.autoPlay) {
                    if (!data.nextState()) {
                        visuals.autoPlay = false
                    }
                    repaint()
                    Thread.sleep(visuals.autoPlayTimeout.toLong())
                }
            }
        }

        updateAction()
        repaint()
    }

    internal fun addStepData(step: Int, stepData: StepData) {
        data.stepDatas[step] = stepData
        if (data.currentStepNumber == null) {
            data.currentStepNumber = step
        }
        repaint()
    }
}
