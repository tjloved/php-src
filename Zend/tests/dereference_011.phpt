--TEST--
Testing array dereference with chaining
--FILE--
<?php

class foo
{
    public $arr;
    public function &a()
    {
        return $this->arr;
    }
}
function fn1314093680()
{
    error_reporting(E_ALL);
    $foo = new foo();
    $h =& $foo->a();
    $h[] = 1;
    $h[] = $foo;
    var_dump($foo->a()[1]->arr[1]->a()[1]->arr[1]->arr[0]);
    var_dump($foo->a()[1]);
    var_dump($foo->a()[1]->arr[1]);
}
fn1314093680();
--EXPECTF--
int(1)
object(foo)#%d (1) {
  ["arr"]=>
  &array(2) {
    [0]=>
    int(1)
    [1]=>
    *RECURSION*
  }
}
object(foo)#%d (1) {
  ["arr"]=>
  &array(2) {
    [0]=>
    int(1)
    [1]=>
    *RECURSION*
  }
}
