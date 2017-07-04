--TEST--
Bug #70262 (Accessing array crashes)
--FILE--
<?php

// make the refcount == 2
class A
{
    public function getObj($array)
    {
        $obj = new Stdclass();
        $obj->arr = $array;
        // make the refcount == 3;
        return $obj;
    }
}
function fn839870421()
{
    $array = array();
    $array[] = 1;
    // make this not immutable
    $extra = $array;
    $a = new A();
    $a->getObj($array)->arr[0] = "test";
    //We will get a refcount == 3 array (not a IS_INDIRCT) in ZEND_ASSIGN_DIM_SPEC_VAR_CONST_HANDLER
    var_dump($array);
}
fn839870421();
--EXPECT--
array(1) {
  [0]=>
  int(1)
}
