--TEST--
Closure 018: Assigning lambda to static var and returning by ref
--FILE--
<?php

class foo
{
    public function test(&$x)
    {
        static $lambda;
        $lambda = function &() use(&$x) {
            return $x = $x * $x;
        };
        return $lambda();
    }
}
function fn737519184()
{
    $test = new foo();
    $y = 2;
    var_dump($test->test($y));
    var_dump($x = $test->test($y));
    var_dump($y, $x);
}
fn737519184();
--EXPECTF--
Notice: Only variable references should be returned by reference in %sclosure_018.php on line %d
int(4)

Notice: Only variable references should be returned by reference in %sclosure_018.php on line %d
int(16)
int(16)
int(16)
