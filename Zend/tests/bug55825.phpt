--TEST--
Bug #55825 (Missing initial value of static locals in trait methods)
--FILE--
<?php

trait T1
{
    public function inc()
    {
        static $x = 1;
        echo $x++ . "\n";
    }
}
class C
{
    use T1;
}
function fn1273535810()
{
    $c1 = new C();
    $c1->inc();
    $c1->inc();
}
fn1273535810();
--EXPECT--
1
2
