--TEST--
debug_backtrace() from within accessor
--FILE--
<?php

class Foo {
    public $bar {
        get { var_dump(debug_backtrace()); }
        set { var_dump(debug_backtrace()); }
        isset { var_dump(debug_backtrace()); }
        unset { var_dump(debug_backtrace()); }
    }
}

$foo = new Foo;

$foo->bar;
$foo->bar = 'test';
isset($foo->bar);
unset($foo->bar);

?>
--EXPECTF--
array(1) {
  [0]=>
  array(8) {
    ["file"]=>
    string(%d) "%s"
    ["line"]=>
    int(%d)
    ["property"]=>
    string(3) "bar"
    ["accessor"]=>
    string(3) "get"
    ["class"]=>
    string(3) "Foo"
    ["object"]=>
    object(Foo)#1 (1) {
      ["bar"]=>
      NULL
    }
    ["type"]=>
    string(2) "->"
    ["args"]=>
    array(0) {
    }
  }
}
array(1) {
  [0]=>
  array(8) {
    ["file"]=>
    string(%d) "%s"
    ["line"]=>
    int(%d)
    ["property"]=>
    string(3) "bar"
    ["accessor"]=>
    string(3) "set"
    ["class"]=>
    string(3) "Foo"
    ["object"]=>
    object(Foo)#1 (1) {
      ["bar"]=>
      NULL
    }
    ["type"]=>
    string(2) "->"
    ["args"]=>
    array(1) {
      [0]=>
      &string(4) "test"
    }
  }
}
array(1) {
  [0]=>
  array(8) {
    ["file"]=>
    string(%d) "%s"
    ["line"]=>
    int(%d)
    ["property"]=>
    string(3) "bar"
    ["accessor"]=>
    string(5) "isset"
    ["class"]=>
    string(3) "Foo"
    ["object"]=>
    object(Foo)#1 (1) {
      ["bar"]=>
      NULL
    }
    ["type"]=>
    string(2) "->"
    ["args"]=>
    array(0) {
    }
  }
}
array(1) {
  [0]=>
  array(8) {
    ["file"]=>
    string(%d) "%s"
    ["line"]=>
    int(%d)
    ["property"]=>
    string(3) "bar"
    ["accessor"]=>
    string(5) "unset"
    ["class"]=>
    string(3) "Foo"
    ["object"]=>
    object(Foo)#1 (1) {
      ["bar"]=>
      NULL
    }
    ["type"]=>
    string(2) "->"
    ["args"]=>
    array(0) {
    }
  }
}
