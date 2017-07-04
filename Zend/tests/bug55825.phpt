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
function fn2056190950()
{
    $c1 = new C();
    $c1->inc();
    $c1->inc();
}
fn2056190950();
--EXPECT--
1
2
