set breakpoint pending on
set confirm off
file ./holeycc
break holeyc::Err::report
commands
	where
end
break negatron::InternalError::InternalError
commands
	where
end

define p3
  set args p3_tests/$arg0.holeyc -p --
  run
end
