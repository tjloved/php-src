--TEST--
Test interaction of Generator::getReturn() and finally
--FILE--
<?php

function gen1()
{
    try {
        throw new Exception("gen1() throw");
    } finally {
        return 42;
    }
    yield;
}
function gen2()
{
    try {
        return 42;
    } finally {
        throw new Exception("gen2() throw");
    }
    yield;
}
function fn1080241285()
{
    // The exception was discarded, so this works
    $gen = gen1();
    var_dump($gen->getReturn());
    $gen = gen2();
    try {
        // This will throw an exception (from the finally)
        // during auto-priming, so fails
        var_dump($gen->getReturn());
    } catch (Exception $e) {
        echo $e->getMessage(), "\n";
    }
    try {
        // This fails, because the return value was discarded
        var_dump($gen->getReturn());
    } catch (Exception $e) {
        echo $e->getMessage(), "\n";
    }
}
fn1080241285();
--EXPECT--
int(42)
gen2() throw
Cannot get return value of a generator that hasn't returned
