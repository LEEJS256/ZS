# Monster

## 구성

- Character
- AbilitySystemComponent
- AttributeSet

---

## 역할

- 데미지 처리(Delegate)
- 데미지 폰트, 체력 UI
- (예정) 피격 / 사망 처리
- (예정)사망시 경험치,골드,아이템 드랍
---

## 데미지 흐름

Hit
→ GE 적용
→ Execution(데미지 계산 공식 적용)
→ AttributeSet
→ OnDamageTaken
→ UI 출력

---

## 구현 포인트

- Delegate 바인딩
- UI 생성 책임

---

## TODO

- 피격 애니메이션
