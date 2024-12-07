/**
 * Renders the small boards in a 3x3 configuration.
 * Sets up an orthographic projection matching the board dimensions
 * and draws the grid lines with a specified line width and color.
 * Skips drawing lines for the big board borders.
 */
void RenderSmallBoards() {
  const int width = 9;
  const int height = 9;

  glLoadIdentity();
  glOrtho(0, width, 0, height, -1, 1);

  // Draw the grid
  glColor3f(0.2f, 0.2f, 0.2f);
  glLineWidth(1.f);
  glBegin(GL_LINES);

  // horizontal lines
  for (int i = 1; i < width; ++i) {
    if (i % 3 == 0)
      continue;

    glVertex2d(i, 0);
    glVertex2d(i, height);
  }

  // vertical lines
  for (int j = 1; j < height; ++j) {
    if (j % 3 == 0)
      continue;

    glVertex2d(0, j);
    glVertex2d(width, j);
  }

  glEnd();
}

/**
 * Renders the main grid for the big board in a 3x3 configuration.
 * Sets up an orthographic projection matching the board dimensions
 * and draws the grid lines with a specified line width and color.
 */
void RenderBigBoard() {
  const int width = 3;
  const int height = 3;

  glLoadIdentity();
  glOrtho(0, width, 0, height, -1, 1);

  // Draw the grid
  glColor3f(1, 1, 1);
  glLineWidth(2.f);
  glBegin(GL_LINES);

  // horizontal lines
  for (int i = 1; i < width; ++i) {
    glVertex2d(i, 0);
    glVertex2d(i, height);
  }

  // vertical lines
  for (int j = 1; j < height; ++j) {
    glVertex2d(0, j);
    glVertex2d(width, j);
  }

  glEnd();
}

/**
 * Renders an X at the given row and column with the given color.
 * The row and column are assumed to be 0-indexed and to be in the range
 * [0, 2].
 *
 * @param row the row to render the X at
 * @param col the column to render the X at
 * @param color the color to render the X in, as an array of 3 GLfloats
 */
void RenderX(int row, int col, const GLfloat* color, float thickness = 3.f) {
  glColor3fv(color);
  glLineWidth(thickness);
  glBegin(GL_LINES);
  glVertex2f(col + 0.2f, row + 0.2f);
  glVertex2f(col + 0.8f, row + 0.8f);

  glVertex2f(col + 0.2f, row + 0.8f);
  glVertex2f(col + 0.8f, row + 0.2f);
  glEnd();
}

/**
 * Renders an O at the given row and column with the given color.
 * The row and column are assumed to be 0-indexed and to be in the range
 * [0, 2].
 *
 * @param row the row to render the O at
 * @param col the column to render the O at
 * @param color the color to render the O in, as an array of 3 GLfloats
 */
void RenderO(int row, int col, const GLfloat* color, float thickness = 3.f) {
  glColor3fv(color);
  glLineWidth(thickness);
  glBegin(GL_LINE_LOOP);
  for (int i = 0; i <= 100; ++i) {
    const GLfloat angle = 2.0f * 3.14159f * i / 100.0f;
    glVertex2f(col + 0.5f + 0.4f * cos(angle), row + 0.5f + 0.4f * sin(angle));
  }
  glEnd();
}

/**
 * Render a yellow border around the given board
 * to indicate the player can play in this board
 *
 * @param boardPosition the position of the board
 */
void RenderBoardBorder(int boardPosition) {
  const GLfloat yellow[3] = {1.f, 1.f, 0.f};
  glColor3fv(yellow);
  glLineWidth(2.f);

  const int boardSize = 3;
  const int startingRow = 9 - boardPosition / boardSize * boardSize;
  const int startingCol = boardPosition % boardSize * boardSize;

  glBegin(GL_LINE_LOOP);
  glVertex2d(startingCol, startingRow);
  glVertex2d(startingCol + boardSize, startingRow);
  glVertex2d(startingCol + boardSize, startingRow - boardSize);
  glVertex2d(startingCol, startingRow - boardSize);
  glEnd();
}

/**
 * Renders a spinning indicator in the top-right corner to show AI is thinking
 * @param angle The current rotation angle of the spinner in degrees
 */
void RenderThinkingSpinner(float angle) {
  const float size = 0.15f; // Size of the spinner
  const float x = 2.85f;    // X position (top-right corner)
  const float y = 2.85f;    // Y position
  const int segments = 8;   // Number of segments in the spinner

  glLoadIdentity();
  glOrtho(0, 3, 0, 3, -1, 1);

  // Save current matrix
  glPushMatrix();

  // Translate to position and rotate
  glTranslatef(x, y, 0.0f);
  glRotatef(angle, 0.0f, 0.0f, 1.0f);

  // Draw the spinner
  glColor3f(0.7f, 0.7f, 0.7f);
  glLineWidth(2.f);
  glBegin(GL_LINES);
  for (int i = 0; i < segments; ++i) {
    float theta = (float)i * 2.0f * M_PI / segments;
    float alpha = (float)(segments - i) / segments; // Fade out effect

    glColor4f(0.7f, 0.7f, 0.7f, alpha);
    glVertex2f(0.0f, 0.0f);
    glVertex2f(size * cosf(theta), size * sinf(theta));
  }
  glEnd();

  // Restore matrix
  glPopMatrix();
}
