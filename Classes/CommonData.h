#pragma once

/* 背景边长 */
#define BACKGROUND_LENGTH 300
/* 块边长 */
#define BLOCK_LENGTH 70
/* 块间距 */
#define SPACE_LENGTH 4
/* 块总数 */
#define BLOCK_COUNT 16
/* 行块数 */
#define ROW_COUNT 4
/* 列块数 */
#define COL_COUNT BLOCK_COUNT / ROW_COUNT

//tag
#define BACKGROUND_TAG 1
#define BLOCK_LABEL_TAG 1
#define PROP_LAYER_TAG 2
#define PROP_LAYER_LABEL_MESSAGE_TAG 1

//zorder
#define BACKGROUND_ZORDER 1
#define BLOCK_LABEL_ZORDER 1
#define PROP_LAYER_ZORDER 99
#define PROP_LAYER_LABEL_MESSAGE_ZORDER 1

//触屏移动响应的最小距离
#define MIN_STEP 30