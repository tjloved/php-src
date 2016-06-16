--TEST--
Bug #70630 (Closure::call/bind() crash with ReflectionFunction->getClosure())
--FILE--
<?php

class a
{
}
function fn1055452233()
{
    $x = (new ReflectionFunction("substr"))->getClosure();
    $x->call(new a());
}
fn1055452233();
--EXPECTF--
Warning: Cannot rebind scope of closure created by ReflectionFunctionAbstract::getClosure() in %s on line %d
