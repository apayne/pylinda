(use-module (pylinda linda))

(define (pong-ping)
    (sequence (in uts (list 'ping))
              (out uts (list 'pong))
    )
)

(pong-ping)
