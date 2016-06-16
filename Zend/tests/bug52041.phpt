--TEST--
Bug #52041 (Memory leak when writing on uninitialized variable returned from function)
--FILE--
<?php

function foo()
{
    return $x;
}
function fn1248107859()
{
    foo()->a = 1;
    foo()->a->b = 2;
    foo()->a++;
    foo()->a->b++;
    foo()->a += 2;
    foo()->a->b += 2;
    foo()[0] = 1;
    foo()[0][0] = 2;
    foo()[0]++;
    foo()[0][0]++;
    foo()[0] += 2;
    foo()[0][0] += 2;
    var_dump(foo());
}
fn1248107859();
--EXPECTF--
Notice: Undefined variable: x in %sbug52041.php on line %d

Warning: Creating default object from empty value in %sbug52041.php on line %d

Notice: Undefined variable: x in %sbug52041.php on line %d

Warning: Creating default object from empty value in %sbug52041.php on line %d

Notice: Undefined variable: x in %sbug52041.php on line %d

Warning: Creating default object from empty value in %sbug52041.php on line %d

Notice: Undefined property: stdClass::$a in %sbug52041.php on line %d

Notice: Undefined variable: x in %sbug52041.php on line %d

Notice: Undefined property: stdClass::$a in %sbug52041.php on line %d

Warning: Creating default object from empty value in %sbug52041.php on line %d

Notice: Undefined property: stdClass::$b in %sbug52041.php on line %d

Notice: Undefined variable: x in %sbug52041.php on line %d

Warning: Creating default object from empty value in %sbug52041.php on line %d

Notice: Undefined property: stdClass::$a in %sbug52041.php on line %d

Notice: Undefined variable: x in %sbug52041.php on line %d

Notice: Undefined property: stdClass::$a in %sbug52041.php on line %d

Warning: Creating default object from empty value in %sbug52041.php on line %d

Notice: Undefined property: stdClass::$b in %sbug52041.php on line %d

Notice: Undefined variable: x in %sbug52041.php on line %d

Notice: Undefined variable: x in %sbug52041.php on line %d

Notice: Undefined variable: x in %sbug52041.php on line %d

Notice: Undefined offset: 0 in %sbug52041.php on line %d

Notice: Undefined variable: x in %sbug52041.php on line %d

Notice: Undefined offset: 0 in %sbug52041.php on line %d

Notice: Undefined offset: 0 in %sbug52041.php on line %d

Notice: Undefined variable: x in %sbug52041.php on line %d

Notice: Undefined offset: 0 in %sbug52041.php on line %d

Notice: Undefined variable: x in %sbug52041.php on line %d

Notice: Undefined offset: 0 in %sbug52041.php on line %d

Notice: Undefined offset: 0 in %sbug52041.php on line %d

Notice: Undefined variable: x in %sbug52041.php on line %d
NULL
