--TEST--
unbraced complex variable replacement test (heredoc)
--FILE--
<?php

require_once 'nowdoc.inc';

print <<<ENDOFHEREDOC
This is heredoc test #s $a, $b, $c['c'], and $d->d.

ENDOFHEREDOC;

$x = <<<ENDOFHEREDOC
This is heredoc test #s $a, $b, $c['c'], and $d->d.

ENDOFHEREDOC;

print "{$x}";

?>
--EXPECTF--
Fatal error: Uncaught exception 'ParseException' with message '%s' in %s:6
Stack trace:
#0 {main}
  thrown in %s on line 6
