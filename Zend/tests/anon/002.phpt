--TEST--
declare anonymous class extending another
--FILE--
<?php

class A
{
}
interface B
{
    public function method();
}
function fn1296406422()
{
    $a = new class extends A implements B
    {
        public function method()
        {
            return true;
        }
    };
    var_dump($a instanceof A, $a instanceof B);
}
fn1296406422();
--EXPECTF--
bool(true)
bool(true)

