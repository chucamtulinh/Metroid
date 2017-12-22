#include "SceneManager.h"

SceneManager* SceneManager::mInstace = NULL;
bool SceneManager::mIsTransitioning = false;

SceneManager::SceneManager()
{
    mCurrentScene = nullptr;
}

SceneManager::~SceneManager()
{
}

SceneManager* SceneManager::GetInstance()
{
    if (!mInstace)
        mInstace = new SceneManager();

    return mInstace;
}

Scene* SceneManager::GetCurrentScene()
{
    return mCurrentScene;
}

void SceneManager::Update(float dt)
{

    if (mIsTransitioning == false)
    {
        mCurrentScene->Update(dt);
    }
    else
    {
        SceneTransition::GetInstance()->Update(dt);
    }
}

void SceneManager::ReplaceScene(Scene *scene)
{
    delete mCurrentScene;

    mCurrentScene = scene;

    OnFinishTransition();
}

void SceneManager::ReplaceScene(Scene * scenedest, TransitionEffect * effect)
{
    mIsTransitioning = true;
    mDestinationScene = scenedest;

    SceneTransition::GetInstance()->SetTransition(mCurrentScene, mDestinationScene, effect);
}

void SceneManager::OnFinishTransition()
{
    mIsTransitioning = false;
}
