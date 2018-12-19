@file:Suppress("MayBeConstant")

package vis

import java.awt.*

internal val SCREEN_SIZE = Toolkit.getDefaultToolkit().screenSize!!
internal val INITIAL_WORLD_WIDTH = SCREEN_SIZE.width - 500
internal val INITIAL_WORLD_HEIGHT = SCREEN_SIZE.height - 160
internal var WORLD_WIDTH = INITIAL_WORLD_WIDTH
internal var WORLD_HEIGHT = INITIAL_WORLD_HEIGHT
internal val MARGIN = 10
internal val WORLD_LOCATION = Point(MARGIN, MARGIN)
internal val PROGRESS_WIDTH get() = WORLD_WIDTH
internal val PROGRESS_HEIGHT = 30
internal val PROGRESS_LOCATION get() = Point(WORLD_LOCATION.x, WORLD_LOCATION.y + WORLD_HEIGHT + MARGIN)
internal val HEADER_WIDTH = 400
internal val HEADER_HEIGHT = 100
internal val HEADER_LOCATION get() = Point(WORLD_LOCATION.x + WORLD_WIDTH + MARGIN, WORLD_LOCATION.y)
internal val TEXTAREA_WIDTH = HEADER_WIDTH
internal val TEXTAREA_HEIGHT get() = WORLD_HEIGHT - HEADER_HEIGHT - MARGIN
internal val TEXTAREA_LOCATION get() = Point(HEADER_LOCATION.x, HEADER_LOCATION.y + HEADER_HEIGHT + MARGIN)

internal val BACKGROUND_COLOR = Color(30, 30, 20)
internal val WORLD_BACKGROUND_COLOR = Color(30, 30, 30)
internal val WORLD_FOREGROUND_COLOR = Color(220, 220, 255)
internal val PROGRESS_COLOR = WORLD_FOREGROUND_COLOR
internal val PROGRESS_CURRENT_STEP_COLOR = Color(200, 0, 0)
internal val HEADER_BACKGROUND_COLOR = Color(0, 0, 0)
internal val HEADER_FOREGROUND_COLOR = Color(240, 240, 255)
internal val TEXTAREA_BACKGROUND_COLOR = HEADER_BACKGROUND_COLOR
internal val TEXTAREA_FOREGROUND_COLOR = HEADER_FOREGROUND_COLOR
internal val FONT_FAMILY = run {
    val families = GraphicsEnvironment.getLocalGraphicsEnvironment().availableFontFamilyNames.toSet()
    listOf("Menlo", "Consolas").firstOrNull { it in families } ?: "Monospaced"
}
internal val WORLD_FONT = Font(FONT_FAMILY, 0, 7)
internal val FOOTER_FONT = Font(FONT_FAMILY, 0, 9)
internal val HEADER_FONT = Font(FONT_FAMILY, 0, 14)
internal val TEXTAREA_FONT = Font(FONT_FAMILY, 0, 9)

internal val ZOOM_FACTOR = 1.2
internal val INITIAL_ZOOM = 0.75

internal val INITIAL_AUTO_PLAY_TIMEOUT = 20.0

internal val INITIAL_SHIFT = Point(0, 0)
