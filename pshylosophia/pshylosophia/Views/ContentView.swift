//
//  ContentView.swift
//  pshylosophia
//
//  Created by Елизавета Сенюк on 30.05.25.
//

import SwiftUI

struct ContentView: View {
    var body: some View {
        NavigationView {
            VStack {
                Spacer()  // отодвигаем содержимое вниз, чтобы заголовок был примерно в центре
                
                Text("Философия")
                    .font(.system(size: 40, weight: .bold, design: .serif))
                    .padding(.bottom, 30) // отступ под заголовком
                
                VStack(spacing: 20) {
                    NavigationLink("Список вопросов", destination: QuestionListView())
                    NavigationLink("Тест", destination: TestView())
                }
                .font(.title2)
                
                Spacer()  // отодвигаем содержимое вверх, чтобы всё было по центру экрана
            }
            .padding()
            .navigationBarHidden(true)  // скрываем стандартный навбар
        }
    }
}
#Preview {
    ContentView()
}
