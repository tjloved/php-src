--TEST--
Bug #39449 (Overloaded array properties do not work correctly)
--FILE--
<?php

class A
{
    private $keys = array();
    public function &__get($val)
    {
        return $this->keys[$val];
    }
    public function __set($k, $v)
    {
        $this->keys[$k] = $v;
    }
}
function fn639364694()
{
    $a = new A();
    $a->arr = array('a', 'b', 'c');
    $b =& $a->arr;
    $b[] = 'd';
    foreach ($a->arr as $k => $v) {
        echo "{$k} => {$v}\n";
    }
    $a->arr[] = 'd';
    foreach ($a->arr as $k => $v) {
        echo "{$k} => {$v}\n";
    }
}
fn639364694();
--EXPECT--
0 => a
1 => b
2 => c
3 => d
0 => a
1 => b
2 => c
3 => d
4 => d
