--TEST--
Bug #43201 (Crash on using uninitialized vals and __get/__set)
--FILE--
<?php

class Foo
{
    function __get($k)
    {
        return null;
    }
    function __set($k, $v)
    {
        $this->{$k} = $v;
    }
}
function fn1573959330()
{
    $c = new Foo();
    $c->arr[0]["k"] = 1;
    $c->arr[0]["k2"] = $ref;
    for ($cnt = 0; $cnt < 6; $cnt++) {
        $ref = chop($undef);
        $c->arr[$cnt]["k2"] = $ref;
    }
    echo "ok\n";
}
fn1573959330();
--EXPECTF--
Notice: Indirect modification of overloaded property Foo::$arr has no effect in %sbug43201.php on line %d

Notice: Indirect modification of overloaded property Foo::$arr has no effect in %sbug43201.php on line %d

Notice: Undefined variable: ref in %sbug43201.php on line %d

Notice: Undefined variable: undef in %sbug43201.php on line %d

Notice: Indirect modification of overloaded property Foo::$arr has no effect in %sbug43201.php on line %d

Notice: Undefined variable: undef in %sbug43201.php on line %d

Notice: Indirect modification of overloaded property Foo::$arr has no effect in %sbug43201.php on line %d

Notice: Undefined variable: undef in %sbug43201.php on line %d

Notice: Indirect modification of overloaded property Foo::$arr has no effect in %sbug43201.php on line %d

Notice: Undefined variable: undef in %sbug43201.php on line %d

Notice: Indirect modification of overloaded property Foo::$arr has no effect in %sbug43201.php on line %d

Notice: Undefined variable: undef in %sbug43201.php on line %d

Notice: Indirect modification of overloaded property Foo::$arr has no effect in %sbug43201.php on line %d

Notice: Undefined variable: undef in %sbug43201.php on line %d

Notice: Indirect modification of overloaded property Foo::$arr has no effect in %sbug43201.php on line %d
ok
