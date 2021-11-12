#include <stdio.h>
#include <math.h>
#include <stdbool.h>

#define BLOCK_COUNT_X 10
#define BLOCK_COUNT_Y 10
#define RESOLUTION_X 100
#define RESOLUTION_Y 40
#define DISTANCE_MAX 10

bool blocks[BLOCK_COUNT_X][BLOCK_COUNT_Y] =
    {{1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
     {1, 0, 0, 0, 1, 0, 0, 0, 0, 1},
     {1, 0, 0, 0, 0, 0, 1, 0, 0, 1},
     {1, 0, 0, 0, 1, 0, 1, 1, 0, 1},
     {1, 0, 0, 0, 1, 0, 0, 1, 0, 1},
     {1, 0, 0, 0, 0, 1, 0, 0, 0, 1},
     {1, 0, 1, 0, 0, 0, 1, 1, 0, 1},
     {1, 0, 0, 0, 0, 0, 0, 1, 0, 1},
     {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
     {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};

static float playerAngle = 0;
const static float playerFoV = M_PI_2;
float playerPositionX = 2.0f;
float playerPositionY = 2.0f;
const float turnspeed = M_PI_4 / 2.0f;

static float calculateDistanceToPlane(float deltaX, float deltaY, float angle)
{
    return (((deltaX)*cosf(angle)) + (deltaY)*sinf(angle));
}

int main(void)
{
    for (;;)
    {

        char dir = '\0';
        scanf("%c", &dir);
        float movementSpeed = 0.0f;

        switch (dir)
        {
        case 'w':
            movementSpeed = 0.5f;
            break;
        case 'a':
            playerAngle -= turnspeed;
            break;
        case 's':
            movementSpeed = -0.5f;
            break;
        case 'd':
            playerAngle += turnspeed;
            break;
        }

        float xMove = cosf(playerAngle) * movementSpeed, yMove = sinf(playerAngle) * movementSpeed;
        float newPlayerPositionX = playerPositionX + xMove, newPlayerPositionY = playerPositionY + yMove;

        if (newPlayerPositionX >= 0.0f && newPlayerPositionX < BLOCK_COUNT_X && newPlayerPositionY >= 0.0f && newPlayerPositionY < BLOCK_COUNT_Y && !blocks[(size_t)(newPlayerPositionX)][(size_t)(newPlayerPositionY)])
        {
            playerPositionX = newPlayerPositionX;
            playerPositionY = newPlayerPositionY;
        }

        double depthBuffer[RESOLUTION_X];
        bool directionBuffer[RESOLUTION_X];

        for (size_t rayNr = 0; rayNr < RESOLUTION_X; rayNr++)
        {
            float rayAngle = (playerAngle - (playerFoV / 2.0f)) + (playerFoV * ((float)rayNr / (float)RESOLUTION_X) + 0.00123f);

            float xComponent = cosf(rayAngle), yComponent = sinf(rayAngle);

            float nearestX = (xComponent > 0) ? ceilf(playerPositionX) : floorf(playerPositionX), nearestY = (yComponent > 0) ? ceilf(playerPositionY) : floorf(playerPositionY);

            float xOffset = 0.0f, yOffset = 0.0f;

            if (xComponent >= 0.0f)
                xOffset = 0.0f;
            else
                xOffset = -1.0f;
            if (yComponent >= 0.0f)
                yOffset = 0.0f;
            else
                yOffset = -1.0f;

            float integerPositionXX = 0.0f, integerPositionXY = 0.0f;
            float integerPositionYX = 0.0f, integerPositionYY = 0.0f;

            if (xComponent != 0.0f)
            {
                integerPositionXX = playerPositionX + xComponent * fabs(nearestX - playerPositionX) / fabs(xComponent);
                integerPositionXY = playerPositionY + yComponent * fabs(nearestX - playerPositionX) / fabs(xComponent);

                integerPositionXX += 0.0001f;
            }
            if (yComponent != 0.0f)
            {
                integerPositionYX = playerPositionX + xComponent * fabs(nearestY - playerPositionY) / fabs(yComponent);
                integerPositionYY = playerPositionY + yComponent * fabs(nearestY - playerPositionY) / fabs(yComponent);

                integerPositionYY += 0.0001f;
            }

            float xUnitStepX = 1.0f, xUnitStepY = 0.0f;
            xUnitStepY = fabs(tanf(rayAngle));
            if (xComponent < 0.0f)
                xUnitStepX = -xUnitStepX;
            if (yComponent < 0.0f)
                xUnitStepY = -xUnitStepY;

            float yUnitStepY = 1.0f, yUnitStepX = 0.0f;
            yUnitStepX = fabs(1.0f / tanf(rayAngle));
            if (xComponent < 0.0f)
                yUnitStepX = -yUnitStepX;
            if (yComponent < 0.0f)
                yUnitStepY = -yUnitStepY;

            bool xWallFound = false, yWallFound = false;
            float xDistance = INFINITY, yDistance = INFINITY;

            while (integerPositionXX + xOffset < BLOCK_COUNT_X && integerPositionXX + xOffset >= 0.0f && integerPositionXY >= 0.0f && integerPositionXY < BLOCK_COUNT_Y && !xWallFound && xUnitStepY != 0.0f)
            {
                if (blocks[(size_t)(integerPositionXX + xOffset)][(size_t)(integerPositionXY)])
                {
                    float deltaX = integerPositionXX - playerPositionX;
                    float deltaY = integerPositionXY - playerPositionY;
                    xDistance = calculateDistanceToPlane(deltaX, deltaY, rayAngle);
                    xWallFound = true;
                    break;
                }
                integerPositionXX += xUnitStepX;
                integerPositionXY += xUnitStepY;
            }

            while (integerPositionYY + yOffset < BLOCK_COUNT_Y && integerPositionYY + yOffset >= 0 && integerPositionYX < BLOCK_COUNT_X && integerPositionYX >= 0.0 && !yWallFound && yUnitStepX != 0.0)
            {
                if (blocks[(size_t)(integerPositionYX)][(size_t)(integerPositionYY + yOffset)])
                {
                    float deltaX = integerPositionYX - playerPositionX;
                    float deltaY = integerPositionYY - playerPositionY;
                    yDistance = calculateDistanceToPlane(deltaX, deltaY, rayAngle);
                    yWallFound = true;
                    break;
                }
                integerPositionYY += yUnitStepY;
                integerPositionYX += yUnitStepX;
            }

            if (xWallFound || yWallFound)
            {
                float distance = xDistance;
                float nonIntegerCoordinate = integerPositionXY;
                directionBuffer[rayNr] = false;

                if (!(xWallFound && (!yWallFound || (xDistance < yDistance))))
                {
                    distance = yDistance;
                    nonIntegerCoordinate = integerPositionYX;
                    directionBuffer[rayNr] = true;
                }

                depthBuffer[rayNr] = distance;
            }
            else
            {
                depthBuffer[rayNr] = INFINITY;
            }
        }

        for (size_t y = 0; y < RESOLUTION_Y; y++)
        {
            for (size_t x = 0; x < RESOLUTION_X; x++)
            {

                int halfWallHeight = RESOLUTION_Y * (0.5 / depthBuffer[x]);
                if (abs(y - (RESOLUTION_Y / 2)) <= halfWallHeight)
                {
                    printf("%c", directionBuffer[x] ? '#' : '%');
                }
                else
                {
                    printf(" ");
                }
            }
            printf("\r\n");
        }
    }
}

