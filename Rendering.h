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
  glBegin(GL_LINES);
  glLineWidth(1);

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
  glBegin(GL_LINES);
  glLineWidth(2);

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
