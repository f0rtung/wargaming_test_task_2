#include <thread>

#include "./nvToolsExt.h"

#include "test.h"
#include "src/point/point.h"
#include "src/SpecialEffectProcessor/SpecialEffectProcessor.h"

SpecialEffectProcessor se_processor;

void WorkerThread(void)
{
    nvtxRangePush(__FUNCTION__);
    se_processor.runProcessing();
    nvtxRangePop();
}


void test::init(void)
{
    std::thread workerThread(WorkerThread);
    workerThread.detach(); // Glut + MSVC = join hangs in atexit()
}

void test::term(void)
{
}

void test::update(int dt)
{
}

void test::render(void)
{
    for (const auto& special_effect : se_processor.getSpecialEffects()) {
        special_effect.draw();
    }
}

void test::on_click(int x, int y)
{
    const RawPoint point{ static_cast<std::uint32_t>(x), static_cast<std::uint32_t>(test::SCREEN_HEIGHT - y) };
    se_processor.addRawPoint(point);
}