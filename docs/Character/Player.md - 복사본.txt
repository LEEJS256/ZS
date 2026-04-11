# PlayerState

## 역할

- AbilitySystemComponent(ASC) 소유
- AttributeSet 관리
- 플레이어의 상태 데이터 유지

---

## 왜 PlayerState에 ASC를 단 이유

### 이유

- PlayerState는 Pawn이 바뀌어도 유지됨
- 네트워크 환경에서 안정적(멀티플레이 지향)
- GAS 권장 구조

---

## 구조

PlayerState
 ├─ AbilitySystemComponent
 ├─ AttributeSet (Player)

Character(Pawn)
 └─ PlayerState 참조

---

## 연결 흐름

플레이어가 Possess 시:

Player
 → PlayerState
 → ASC 초기화(DataAsset 참조)
 → Character와 연결

---

## 역할 분리

| 클래스 | 역할 |
|--------|------|
| PlayerState | 데이터 저장 (ASC, Attribute) |
| Character | 실제 동작 (이동, 공격) |

---

## 데미지 흐름에서 역할

- 공격 시 Source ASC 제공
- GameplayEffect 생성 주체

---

## 장점

- Pawn 교체 시 데이터 유지
- GAS 구조에 적합
- 멀티플레이 확장 용이

---

## TODO

- Ability 추가 (EVADE)