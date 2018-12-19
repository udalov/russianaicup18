package vis

import java.awt.Cursor
import java.awt.Point
import java.awt.Toolkit
import java.awt.event.*
import kotlin.math.pow
import kotlin.math.roundToInt

internal class VisMouseListener(
    private val visuals: VisualState,
    private val data: GameData,
    private val updateCursor: (Cursor) -> Unit,
    private val repaint: () -> Unit
) : MouseListener {
    override fun mouseReleased(e: MouseEvent) {
        visuals.lastWorldMouseLocation = null
        visuals.mousePressedInProgress = false
        updateCursor(Cursor.getDefaultCursor())
    }

    override fun mouseEntered(e: MouseEvent) = Unit

    override fun mouseClicked(e: MouseEvent) = Unit

    override fun mouseExited(e: MouseEvent) = Unit

    override fun mousePressed(e: MouseEvent) {
        val point = e.point
        if (isInsideWorld(point)) {
            visuals.lastWorldMouseLocation = point
            updateCursor(Cursor(Cursor.HAND_CURSOR))
        } else if (isInsideProgress(point)) {
            visuals.mousePressedInProgress = true
            data.setCurrentStateByX(point.x)
            repaint()
        }
    }
}

internal class VisMouseMotionListener(
    private val visuals: VisualState,
    private val data: GameData,
    private val repaint: () -> Unit
) : MouseMotionListener {
    override fun mouseMoved(e: MouseEvent) = Unit

    override fun mouseDragged(e: MouseEvent) {
        visuals.lastWorldMouseLocation?.let { l ->
            visuals.shift.translate(e.x - l.x, e.y - l.y)
            visuals.lastWorldMouseLocation = e.point
            repaint()
        } ?: if (visuals.mousePressedInProgress) {
            data.setCurrentStateByX(e.x)
            repaint()
        }
    }
}

private fun GameData.setCurrentStateByX(x: Int) {
    val ratio = ((x - PROGRESS_LOCATION.x).toDouble() / PROGRESS_WIDTH).coerceIn(0.0, 1.0)
    val next = (ratio * (stepDatas.size - 1)).roundToInt()
    if (stepDatas.containsKey(next)) {
        currentStepNumber = next
    }
}

internal class VisMouseWheelListener(
    private val visuals: VisualState,
    private val repaint: () -> Unit
) : MouseWheelListener {
    override fun mouseWheelMoved(e: MouseWheelEvent) {
        val point = e.point
        if (isInsideWorld(point)) {
            val factor = ZOOM_FACTOR.pow(-e.preciseWheelRotation)

            // Let px = point.x, wx = WORLD_LOCATION.x, sx = shift.y and the task is to find sdx
            // (px - wx - sx) / zoom = (px - wx - sx - sdx) / (zoom * factor)
            // (px - wx - sx) * factor = px - wx - sx - sdx
            // sdx = (px - wx - sx) * (1 - factor)
            val shiftDX = (point.x - WORLD_LOCATION.x - visuals.shift.x) * (1 - factor)
            val shiftDY = (point.y - WORLD_LOCATION.y - visuals.shift.y) * (1 - factor)

            visuals.zoom *= factor
            visuals.shift.translate(shiftDX.toInt(), shiftDY.toInt())

            repaint()
        }
    }
}

internal class VisKeyListener(
    private val visuals: VisualState,
    private val data: GameData,
    private val updateAction: () -> Unit,
    private val repaint: () -> Unit
) : KeyListener {
    override fun keyTyped(e: KeyEvent) = Unit

    override fun keyPressed(e: KeyEvent) {
        if (handleKeyPressed(e, visuals, data, updateAction)) {
            repaint()
        }
    }

    override fun keyReleased(e: KeyEvent) {
        if (handleKeyReleased(e, visuals)) {
            repaint()
        }
    }
}

private fun handleKeyPressed(e: KeyEvent, visuals: VisualState, data: GameData, updateAction: () -> Unit): Boolean {
    val shift = visuals.shift
    val zoom = visuals.zoom
    val cmdKey = Toolkit.getDefaultToolkit().menuShortcutKeyMask

    return when (e.keyCode) {
        KeyEvent.VK_W -> true.apply { shift.translate(0, -(100 * zoom).toInt()) }
        KeyEvent.VK_S -> true.apply { shift.translate(0, (100 * zoom).toInt()) }
        KeyEvent.VK_A -> true.apply { shift.translate(-(100 * zoom).toInt(), 0) }
        KeyEvent.VK_D -> true.apply { shift.translate((100 * zoom).toInt(), 0) }
        KeyEvent.VK_0 -> true.apply { visuals.zoom = 1.0 }
        KeyEvent.VK_EQUALS -> true.apply { visuals.zoom *= ZOOM_FACTOR }
        KeyEvent.VK_MINUS -> true.apply { visuals.zoom /= ZOOM_FACTOR }
        KeyEvent.VK_UP -> true.apply { visuals.autoPlayTimeout /= 2 }
        KeyEvent.VK_DOWN -> true.apply { visuals.autoPlayTimeout *= 2 }
        KeyEvent.VK_SPACE -> true.apply { visuals.autoPlay = !visuals.autoPlay }
        KeyEvent.VK_LEFT -> true.apply { if (e.modifiers == cmdKey) data.currentStepNumber = 0 else data.previousState() }
        KeyEvent.VK_RIGHT -> true.apply {
            if (e.modifiers == cmdKey) data.currentStepNumber = data.stepDatas.size - 1 else data.nextState()
        }
        KeyEvent.VK_R -> true.apply { updateAction() }
        KeyEvent.VK_ALT -> true.apply { visuals.drawExtras = true }
        else -> false
    }
}

private fun handleKeyReleased(e: KeyEvent, visuals: VisualState): Boolean {
    return when (e.keyCode) {
        KeyEvent.VK_ALT -> true.apply { visuals.drawExtras = false }
        else -> false
    }
}

private fun isInsideWorld(point: Point) =
    WORLD_LOCATION.x <= point.x && point.x <= WORLD_LOCATION.x + WORLD_WIDTH &&
            WORLD_LOCATION.y <= point.y && point.y <= WORLD_LOCATION.y + WORLD_HEIGHT

private fun isInsideProgress(point: Point) =
    PROGRESS_LOCATION.x <= point.x && point.x <= PROGRESS_LOCATION.x + PROGRESS_WIDTH &&
            PROGRESS_LOCATION.y <= point.y && point.y <= PROGRESS_LOCATION.y + PROGRESS_HEIGHT
