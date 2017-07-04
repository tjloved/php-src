--TEST--
Generator methods can yield by reference
--FILE--
<?php

class Test implements IteratorAggregate
{
    protected $data;
    public function __construct(array $data)
    {
        $this->data = $data;
    }
    public function getData()
    {
        return $this->data;
    }
    public function &getIterator()
    {
        foreach ($this->data as $key => &$value) {
            (yield $key => $value);
        }
    }
}
function fn174205722()
{
    $test = new Test([1, 2, 3, 4, 5]);
    foreach ($test as &$value) {
        $value *= -1;
    }
    var_dump($test->getData());
}
fn174205722();
--EXPECT--
array(5) {
  [0]=>
  int(-1)
  [1]=>
  int(-2)
  [2]=>
  int(-3)
  [3]=>
  int(-4)
  [4]=>
  &int(-5)
}
