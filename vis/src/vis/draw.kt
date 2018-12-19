package vis

import java.awt.Graphics2D
import kotlin.math.roundToInt

internal fun Graphics2D.draw(visuals: VisualState, data: GameData) {
    val worldX = WORLD_LOCATION.x
    val worldY = WORLD_LOCATION.y
    color = WORLD_BACKGROUND_COLOR
    fillRect(worldX + 1, worldY + 1, WORLD_WIDTH - 2, WORLD_HEIGHT - 2)
    color = WORLD_FOREGROUND_COLOR
    drawRect(worldX, worldY, WORLD_WIDTH, WORLD_HEIGHT)

    font = WORLD_FONT
    drawClipped(worldX, worldY, WORLD_WIDTH, WORLD_HEIGHT) {
        drawTranslated(worldX, worldY) {
            drawTranslated(visuals.shift.x, visuals.shift.y) {
                scale(visuals.zoom, visuals.zoom)
                drawWorld(visuals, data.currentStepData)
            }
        }
    }

    drawTranslated(PROGRESS_LOCATION.x, PROGRESS_LOCATION.y) {
        drawProgress(data)
    }

    drawTranslated(HEADER_LOCATION.x, HEADER_LOCATION.y) {
        drawHeader(data.currentStepData)
    }

    drawTranslated(TEXTAREA_LOCATION.x, TEXTAREA_LOCATION.y) {
        drawTextarea(data.currentStepData)
    }

    font = FOOTER_FONT
    drawTranslated(worldX + 2, worldY + WORLD_HEIGHT - 2) {
        drawString("zoom = ${String.format("%.4f", visuals.zoom)}", 0, -font.size - 2)
        drawString("speed = ${String.format("%.4f", 50.0 / visuals.autoPlayTimeout)}", 0, 0)
    }
}

private fun Graphics2D.drawProgress(data: GameData) {
    color = PROGRESS_COLOR
    drawLine(0, PROGRESS_HEIGHT / 2, PROGRESS_WIDTH, PROGRESS_HEIGHT / 2)

    for (i in data.stepDatas.keys) {
        val x = (i.toDouble() / (data.stepDatas.size - 1) * PROGRESS_WIDTH).roundToInt()
        if (i == data.currentStepNumber) {
            color = PROGRESS_CURRENT_STEP_COLOR
            fillRect(x - 5, 0, 10, PROGRESS_HEIGHT)
        } else {
            color = PROGRESS_COLOR
            drawLine(x, PROGRESS_HEIGHT / 3, x, PROGRESS_HEIGHT * 2 / 3)
        }
    }

    color = PROGRESS_COLOR
    font = FOOTER_FONT
    drawString("${data.currentStepNumber} / ${data.stepDatas.size - 1}", 0, PROGRESS_HEIGHT + 2 + font.size)
}

private fun Graphics2D.drawHeader(data: StepData?) {
    color = HEADER_BACKGROUND_COLOR
    fillRect(0, 0, HEADER_WIDTH, HEADER_HEIGHT)
    color = HEADER_FOREGROUND_COLOR
    drawRect(1, 1, HEADER_WIDTH - 1, HEADER_HEIGHT - 1)

    data?.header?.let { header ->
        font = HEADER_FONT
        drawClipped(1, 1, HEADER_WIDTH - 1, HEADER_HEIGHT - 1) {
            for ((index, string) in header.withIndex()) {
                drawString(string, 4, (4 + font.size) * (index + 1))
            }
        }
    }
}

private fun Graphics2D.drawTextarea(data: StepData?) {
    color = TEXTAREA_BACKGROUND_COLOR
    fillRect(0, 0, TEXTAREA_WIDTH, TEXTAREA_HEIGHT)
    color = TEXTAREA_FOREGROUND_COLOR
    drawRect(1, 1, TEXTAREA_WIDTH - 1, TEXTAREA_HEIGHT - 1)

    data?.text?.let { text ->
        font = TEXTAREA_FONT
        drawClipped(1, 1, TEXTAREA_WIDTH - 1, TEXTAREA_HEIGHT - 1) {
            for ((index, string) in text.withIndex()) {
                drawString(string, 4, (4 + font.size) * (index + 1))
            }
        }
    }
}

private fun Graphics2D.drawWorld(visuals: VisualState, data: StepData?) {
    data?.drawWorld?.accept(this)
    if (visuals.drawExtras) {
        data?.drawExtras?.accept(this)
    }
}
